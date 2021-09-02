#include "application.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "render/buffer/vertex.hpp"
#include <chrono>
namespace painting
{
    void PaintingApplication::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<PaintingApplication *>(glfwGetWindowUserPointer(window));
        app->framebuffer_resized_ = true;
    }

    void PaintingApplication::run(uint32_t width, uint32_t height, std::string title)
    {
        init_window(width, height, title);
        init_device();
        command_pool_ = std::make_unique<vkcpp::CommandPool>(device_.get(), 0, device_->get_gpu().get_queue_family_indices().graphics_family.value());
        init_render();
        object_ = std::make_unique<vkcpp::Object>(device_.get(), render_stage_.get(), command_pool_.get());
        init_frame();
        init_synobj();
        main_loop();
        cleanup();
    }
    void PaintingApplication::init_window(uint32_t width, uint32_t height, std::string title)
    {
        vkcpp::MainWindow::getInstance()->set_window(width, height, title);
        vkcpp::MainWindow::getInstance()->set_user_pointer(this);
        vkcpp::MainWindow::getInstance()->set_framebuffer_size_callback(framebufferResizeCallback);

        instance_ = std::make_unique<vkcpp::Instance>();
        surface_ = std::make_unique<vkcpp::Surface>(instance_.get(), vkcpp::MainWindow::getInstance()->create_surface(*instance_));
    }

    void PaintingApplication::init_render()
    {
        swapchain_ = std::make_unique<vkcpp::Swapchain>(device_.get(), surface_.get());
        render_stage_ = std::make_unique<vkcpp::RenderStage>(device_.get(), swapchain_.get());
        command_buffers_ = std::make_unique<vkcpp::CommandBuffers>(device_.get(), command_pool_.get(), swapchain_->get_image_views().size(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }
    void PaintingApplication::init_synobj()
    {

        image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
        in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
        images_in_flight_.resize(swapchain_->get_image_views().size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(*device_, &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
                vkCreateSemaphore(*device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
                vkCreateFence(*device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }
    void PaintingApplication::init_device()
    {
        vkcpp::PhysicalDevice *gpu = instance_->get_suitable_gpu(*surface_, device_extensions_);

        /*
         Request to enable ASTC
        if (gpu.get_features().textureCompressionASTC_LDR)
        {
            gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
        }
        */

        device_ = std::make_unique<vkcpp::Device>(gpu, surface_.get());
    }
    void PaintingApplication::init_frame()
    {
        size_t size = command_buffers_->get_command_buffers().size();
        for (size_t i = 0; i < size; i++)
        {
            command_buffers_->begin_command_buffer(i, 0);

            command_buffers_->begin_render_pass(i, render_stage_.get());

            object_->draw((*command_buffers_)[i], i);

            command_buffers_->end_render_pass(i, render_stage_.get());

            command_buffers_->end_command_buffer(i);
        }
    }
    void PaintingApplication::update_uniform_buffer(uint32_t idx)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        vkcpp::TransformUBO ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), render_stage_->get_render_area().extent.width / (float)render_stage_->get_render_area().extent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;
        object_->get_mutable_uniform_buffers().update_uniform_buffer(idx, ubo);
    }
    void PaintingApplication::draw_frame()
    {
        vkWaitForFences(*device_, 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

        uint32_t image_index;
        VkResult result = vkAcquireNextImageKHR(*device_, *swapchain_, UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreate_swapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        update_uniform_buffer(image_index);

        if (images_in_flight_[image_index] != VK_NULL_HANDLE)
        {
            vkWaitForFences(*device_, 1, &images_in_flight_[image_index], VK_TRUE, UINT64_MAX);
        }
        images_in_flight_[image_index] = in_flight_fences_[current_frame_];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_available_semaphores_[current_frame_]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &(*command_buffers_)[image_index];

        VkSemaphore signalSemaphores[] = {render_finished_semaphores_[current_frame_]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(*device_, 1, &in_flight_fences_[current_frame_]);

        if (vkQueueSubmit(device_->get_graphics_queue(), 1, &submitInfo, in_flight_fences_[current_frame_]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {*swapchain_};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &image_index;
        presentInfo.pResults = nullptr; // Optional

        result = vkQueuePresentKHR(device_->get_present_queue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_)
        {
            framebuffer_resized_ = false;
            recreate_swapchain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    void PaintingApplication::main_loop()
    {
        while (!vkcpp::MainWindow::getInstance()->should_close())
        {
            vkcpp::MainWindow::getInstance()->process_events();
            draw_frame();
        }
    }

    void PaintingApplication::cleanup()
    {
        vkDeviceWaitIdle(*device_);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(*device_, render_finished_semaphores_[i], nullptr);
            vkDestroySemaphore(*device_, image_available_semaphores_[i], nullptr);
            vkDestroyFence(*device_, in_flight_fences_[i], nullptr);
        }

        cleanup_swapchain();

        object_.reset();
        command_pool_.reset();
        device_.reset();
        surface_.reset();
        instance_.reset();
        vkcpp::MainWindow::getInstance()->destroy_window();
    }
    void PaintingApplication::cleanup_swapchain()
    {
        object_->destroy_dependency_swapchain();
        command_buffers_.reset();
        render_stage_.reset();
        swapchain_.reset();
    }

    void PaintingApplication::recreate_swapchain()
    {
        auto size = vkcpp::MainWindow::getInstance()->get_framebuffer_size();

        while (size.first == 0 || size.second == 0)
        {
            size = vkcpp::MainWindow::getInstance()->get_framebuffer_size();
            vkcpp::MainWindow::getInstance()->wait_events();
        }

        vkDeviceWaitIdle(*device_);

        cleanup_swapchain();

        init_render();
        object_->init_dependency_swapchain(render_stage_.get());
        init_frame();

        images_in_flight_.resize(swapchain_->get_image_views().size(), VK_NULL_HANDLE);
    }

} // namespace painting