#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include "application.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "object/shader_attribute.hpp"
#include "utility/create.h"
#include "device/queue.h"
#include <fstream>
#include <chrono>
#include <algorithm>

namespace painting
{

    void PaintingApplication::run(uint32_t width, uint32_t height, std::string title)
    {
        init_window(width, height, title);
        init_device();
        command_pool_ = std::make_unique<vkcpp::CommandPool>(device_.get(), device_->get_graphics_queue(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        init_render();
        init_synobj();
        main_loop();
        cleanup();
    }

    void PaintingApplication::init_offscreen(VkExtent3D extent)
    {
        int size = swapchain_->get_image_views().size();
        offscreens_ = std::make_unique<vkcpp::Offscreens>(device_.get(), extent, size);
        offscreen_render_stage_ = std::make_unique<vkcpp::RenderStage>(device_.get(), offscreens_.get());
        brush_.emplace_back(std::make_unique<Picture>(device_.get(), offscreen_render_stage_.get(), command_pool_.get(), extent));
        brush_.emplace_back(std::make_unique<Brush>(device_.get(), offscreen_render_stage_.get(), command_pool_.get(), 0));
    }

    void PaintingApplication::init_window(uint32_t width, uint32_t height, std::string title)
    {
        vkcpp::MainWindow::getInstance()->set_window(width, height, title);
        vkcpp::MainWindow::getInstance()->set_user_pointer(this);
        vkcpp::MainWindow::getInstance()->set_framebuffer_size_callback(framebufferResizeCallback);
        vkcpp::MainWindow::getInstance()->set_drop_callback(dropCallback);

        instance_ = std::make_unique<vkcpp::Instance>();
        surface_ = std::make_unique<vkcpp::Surface>(instance_.get(), vkcpp::MainWindow::getInstance()->create_surface(*instance_));
    }

    void PaintingApplication::init_render()
    {
        swapchain_ = std::make_unique<vkcpp::Swapchain>(device_.get(), surface_.get());
        render_stage_ = std::make_unique<vkcpp::RenderStage>(device_.get(), swapchain_.get());
        command_buffers_ = std::make_unique<vkcpp::CommandBuffers>(device_.get(), command_pool_.get(), swapchain_->get_image_views().size(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        is_command_buffer_updated_.resize(swapchain_->get_image_views().size(), false);
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
        instance_->query_gpus(surface_.get());
        vkcpp::PhysicalDevice *gpu = instance_->get_suitable_gpu(device_extensions_);

        /*
         Request to enable ASTC
        if (gpu.get_features().textureCompressionASTC_LDR)
        {
            gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
        }
        */

        device_ = std::make_unique<vkcpp::Device>(gpu);
    }

    void PaintingApplication::record_command_buffers()
    {
        uint32_t size = command_buffers_->size();
        for (int i = 0; i < size; i++)
        {
            record_command_buffer(i);
        }
    }

    void PaintingApplication::record_command_buffer(int idx)
    {
        command_buffers_->begin_command_buffer(idx, 0);

        command_buffers_->begin_render_pass(idx, render_stage_.get());

        for (int j = 0; j < object_.size(); j++)
        {
            object_[j]->draw((*command_buffers_)[idx], idx);
        }
        for (int j = 0; j < brush_.size(); j++)
        {
            brush_[j]->draw((*command_buffers_)[idx], idx); //object_[0]->get_graphics_pipeline(), idx);
        }

        command_buffers_->end_render_pass(idx, render_stage_.get());

        command_buffers_->end_command_buffer(idx);
        is_command_buffer_updated_[idx] = true;
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

        for (int i = 0; i < object_.size(); i++)
        {
            object_[i]->update(image_index);
        }
        for (int i = 0; i < brush_.size(); i++)
        {
            brush_[i]->update(image_index);
        }

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

        if (!is_command_buffer_updated_[image_index])
        {
        }
        record_command_buffer(image_index);
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &(*command_buffers_)[image_index];

        VkSemaphore signalSemaphores[] = {render_finished_semaphores_[current_frame_]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(*device_, 1, &in_flight_fences_[current_frame_]);

        if (vkQueueSubmit(*(device_->get_graphics_queue()), 1, &submitInfo, in_flight_fences_[current_frame_]) != VK_SUCCESS)
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

        result = vkQueuePresentKHR(*(device_->get_present_queue()), &presentInfo);

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
        auto current_time = std::chrono::high_resolution_clock::now();
        while (!vkcpp::MainWindow::getInstance()->should_close())
        {
            vkcpp::MainWindow::getInstance()->process_events();

            auto new_time = std::chrono::high_resolution_clock::now();
            float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
            current_time = new_time;

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
        for (auto &obj_ptr : brush_)
        {
            obj_ptr.reset();
        }

        brush_.resize(0);
        offscreen_render_stage_.reset();
        offscreens_.reset();

        cleanup_swapchain();

        for (int i = 0; i < object_.size(); i++)
        {
            object_[i].reset();
        }
        object_.resize(0);
        command_pool_.reset();
        device_.reset();
        surface_.reset();
        instance_.reset();
        vkcpp::MainWindow::getInstance()->destroy_window();
    }

    void PaintingApplication::cleanup_swapchain()
    {
        for (int i = 0; i < object_.size(); i++)
        {
            object_[i]->destroy_dependency_renderpass();
        }
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
        // TODO check renderpass compatibility
        // If the number of new swapchain images is the same as the old image,
        // there is no need to change the number of buffers.(commandbuffer and etc.)
        for (int i = 0; i < object_.size(); i++)
        {
            object_[i]->init_dependency_renderpass(render_stage_.get());
        }
        record_command_buffers();

        images_in_flight_.resize(swapchain_->get_image_views().size(), VK_NULL_HANDLE);
    }
    void PaintingApplication::push_object(const char *texture_file)
    {
        object_.emplace_back(std::make_unique<vkcpp::Object2D>(device_.get(), render_stage_.get(), command_pool_.get(), texture_file));
        //object_.emplace_back(std::make_unique<Brush>(device_.get(), render_stage_.get(), command_pool_.get(), 0));
        init_offscreen(object_[0]->get_extent_3d());
    }
    void PaintingApplication::reset_command_buffers_update_flag()
    {
        is_command_buffer_updated_ = std::move(std::vector<bool>(is_command_buffer_updated_.size(), false));
    }

    // Take a screenshot from the current swapchain image
    // This is done using a blit from the swapchain image to a linear image whose memory content is then saved as a ppm image
    // Getting the image date directly from a swapchain image wouldn't work as they're usually stored in an implementation dependent optimal tiling format
    // Note: This requires the swapchain images to be created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag (see VulkanSwapChain::create)
    void PaintingApplication::save_screen_shot(const char *filename, VkFormat swapchain_color_format, VkImage src_image, VkExtent3D extent)
    {
        const vkcpp::Device *device = device_.get();
        const vkcpp::CommandPool *command_pool = command_pool_.get();
        bool supportsBlit = device_->check_support_blit(swapchain_color_format);

        // Source for the copy is the last rendered swapchain image
        // VkImage srcImage = swapChain.images[currentBuffer];

        // Create the linear tiled destination image to copy to and to read the memory from
        // Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
        VkImage dst_image;
        // Create memory to back up the image
        VkDeviceMemory dst_image_memory{nullptr};
        vkcpp::create::image(device,
                             VK_IMAGE_TYPE_2D,
                             VK_FORMAT_R8G8B8A8_UNORM,
                             extent,
                             VK_IMAGE_TILING_LINEAR,
                             VK_SAMPLE_COUNT_1_BIT,
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // Memory must be host visible to copy from
                             dst_image,
                             dst_image_memory);

        // Do the actual blit from the swapchain image to our host visible destination image
        vkcpp::CommandBuffers copy_cmd = std::move(vkcpp::CommandBuffers::beginSingleTimeCmd(device, command_pool));
        // begin
        // Transition destination image to transfer destination layout
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            dst_image,
            0,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition swapchain image from present to transfer source layout
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            src_image,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        vkcpp::CommandBuffers::cmdCopyImage(
            copy_cmd[0],
            supportsBlit,
            extent,
            {VK_IMAGE_ASPECT_COLOR_BIT, 1}, {VK_IMAGE_ASPECT_COLOR_BIT, 1},
            src_image, dst_image);

        // Transition destination image to general layout, which is the required layout for mapping the image memory later on
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            dst_image,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_GENERAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        // Transition back the swap chain image after the blit is done
        vkcpp::CommandBuffers::cmdImageMemoryBarrier(
            copy_cmd[0],
            src_image,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

        copy_cmd.flush_command_buffer(0);

        // Get layout of the image (including row pitch)
        VkImageSubresource subResource{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
        VkSubresourceLayout subResourceLayout;
        vkGetImageSubresourceLayout(*device, dst_image, &subResource, &subResourceLayout);

        // Map image memory so we can start copying from it
        const char *data;
        vkMapMemory(*device, dst_image_memory, 0, VK_WHOLE_SIZE, 0, (void **)&data);
        data += subResourceLayout.offset;

        std::ofstream file(filename, std::ios::out | std::ios::binary);

        // ppm header
        file << "P6\n"
             << extent.width << "\n"
             << extent.height << "\n"
             << 255 << "\n";

        // If source is BGR (destination is always RGB) and we can't use blit (which does automatic conversion), we'll have to manually swizzle color components
        bool colorSwizzle = false;
        // Check if source is BGR
        // Note: Not complete, only contains most common and basic BGR surface formats for demonstration purposes
        if (!supportsBlit)
        {
            std::vector<VkFormat> formatsBGR = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
            colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), swapchain_color_format) != formatsBGR.end());
        }

        // ppm binary pixel data
        for (uint32_t y = 0; y < extent.height; y++)
        {
            unsigned int *row = (unsigned int *)data;
            for (uint32_t x = 0; x < extent.width; x++)
            {
                if (colorSwizzle)
                {
                    file.write((char *)row + 2, 1);
                    file.write((char *)row + 1, 1);
                    file.write((char *)row, 1);
                }
                else
                {
                    file.write((char *)row, 3);
                }
                row++;
            }
            data += subResourceLayout.rowPitch;
        }
        file.close();

        std::cout << "Screenshot saved to disk" << std::endl;

        // Clean up resources
        vkUnmapMemory(*device, dst_image_memory);
        vkFreeMemory(*device, dst_image_memory, nullptr);
        vkDestroyImage(*device, dst_image, nullptr);
    }
} // namespace painting
/**
 * call back
 */
namespace painting
{
    void PaintingApplication::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<PaintingApplication *>(glfwGetWindowUserPointer(window));
        app->framebuffer_resized_ = true;
    }

    void PaintingApplication::dropCallback(GLFWwindow *window, int count, const char **paths)
    {
        auto app = reinterpret_cast<PaintingApplication *>(glfwGetWindowUserPointer(window));
        if (count > 0)
        {
            app->reset_command_buffers_update_flag();
        }
        for (int i = 0; i < count; i++)
        {
            if (app->object_.size() == 0)
            {
                app->push_object(paths[i]);
            }
            else
            {
                app->object_[0]->sub_texture(paths[i]);
            }
        }
    }

}