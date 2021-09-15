#include "class/picture.h"

#include "device/device.h"
#include "device/queue.h"
#include "render/image/image.h"
#include "render/image/image2d.h"
#include "render/render_stage.h"
#include "render/swapchain/swapchain.h"
#include "render/command/command_pool.h"
#include "render/pipeline/graphics_pipeline.h"

namespace painting
{
    Picture::Picture(const vkcpp::Device *device,
                     const vkcpp::CommandPool *command_pool,
                     const vkcpp::RenderStage *render_stage,
                     const VkExtent3D &extent,
                     uint32_t swapchain_image_size,
                     uint32_t population_size,
                     uint32_t brush_count)
    {
        device_ = device;
        offscreens_image_size_ = swapchain_image_size;
        extent_ = extent;
        command_pool_ = command_pool;

        offscreens_ = std::make_unique<vkcpp::Offscreens>(device_, command_pool_, extent, swapchain_image_size);
        offscreen_render_stage_ = std::make_unique<vkcpp::RenderStage>(device_, offscreens_.get());
        render_stage_ = offscreen_render_stage_.get();

        command_buffers_ = std::make_unique<vkcpp::CommandBuffers>(device_, command_pool_, swapchain_image_size, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        is_command_buffer_updated_.resize(swapchain_image_size, false);

        brushes_ = std::make_unique<Brushes>(device, render_stage_, command_pool_, brush_count);

        population_ = std::make_unique<Population>(glm::vec2(0.0f, 0.0f),
                                                   glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height)),
                                                   glm::vec2(0.003f, 0.03f),
                                                   BrushAttributes::Probablity(0.5f, 0.05f, 0.8f, 0.5f),
                                                   population_size,
                                                   brush_count);
        camera_ = std::make_unique<vkcpp::SubCamera>(
            extent);

        width_ = static_cast<float>(extent.width);
        height_ = static_cast<float>(extent.height);

        camera_->update_proj_to_ortho({0.0f, width_},
                                      {0.0f, height_},
                                      {-100.0f, 100.0f});

        init_texture(extent, VK_FORMAT_R8G8B8A8_SRGB);
        init_object2d();

        init_synobj();
        record_command_buffers();

        //    auto [img1, mem1, dt1, rowpitch1] = map_read_image_memory();
        //    data_to_file("first.ppm", dt1, extent_, get_format(), false, rowpitch1);
        //    unmap_buffer_memory(img1, mem1);
    }
    Picture::~Picture()
    {
        wait_thread();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++)
        {
            vkDestroySemaphore(*device_, image_available_semaphores_[i], nullptr);
            vkDestroySemaphore(*device_, render_finished_semaphores_[i], nullptr);
        }
        for (int i = 0; i < offscreens_image_size_; i++)
        {
            vkDestroyFence(*device_, in_flight_fences_[i], nullptr);
        }
    }
    void Picture::wait_thread()
    {
        for (int i = 0; i < MAX_THREAD_; i++)
        {
            if (frame_thread_[i].joinable())
            {
                frame_thread_[i].join();
            }
        }
    }
    void Picture::record_command_buffers()
    {
        uint32_t size = command_buffers_->size();
        for (int i = 0; i < size; i++)
        {
            record_command_buffer(i);
        }
    }
    void Picture::record_command_buffer(int idx)
    {
        command_buffers_->begin_command_buffer(idx, 0);

        command_buffers_->begin_render_pass(idx, render_stage_);
        bind_graphics_pipeline((*command_buffers_)[idx]);
        draw((*command_buffers_)[idx], idx);
        brushes_->draw_all((*command_buffers_)[idx], idx);

        command_buffers_->end_render_pass(idx, render_stage_);

        command_buffers_->end_command_buffer(idx);
        is_command_buffer_updated_[idx] = true;
    }
    void Picture::run(const char *data)
    {
        int size = population_->get_size();
        population_->next_stage();
        for (int i = 0; i < size; i++)
        {
            draw_frame(i, data, false);
            current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT_;
            image_index_ = (image_index_ + 1) % offscreens_image_size_;
            thread_index_ = (thread_index_ + 1) % MAX_THREAD_;
        }
        wait_thread();
        population_->sort();

        if (population_->get_mutable_fitness(0) >= population_->get_best() - 0.001)
        {
            draw_frame(0, data, true);
            population_->set_best(population_->get_mutable_fitness(0));

            offscreens_->get_mutable_offscreen(image_index_).screen_to_image(command_pool_, get_image(), extent_, VK_FORMAT_B8G8R8A8_SRGB);
        }
        vkQueueWaitIdle(*device_->get_graphics_queue());
    }

    void Picture::draw_frame(int population_idx, const char *data, bool is_top)
    {
        if (frame_thread_[image_index_].joinable())
        {
            frame_thread_[image_index_].join();
        }

        init_transform({width_ / 2.0f, height_ / 2.0f, -90.0f});
        update_with_sub_camera(image_index_, camera_.get());
        int brushes_size = brushes_->get_brushes_size();
        for (int i = 0; i < brushes_size; i++)
        {
            brushes_->update(population_->get(population_idx)->get_attribute(i), camera_.get(), i, image_index_);
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &(*command_buffers_)[image_index_];

        vkResetFences(*device_, 1, &in_flight_fences_[current_frame_]);
        device_->graphics_queue_submit(&submitInfo, 1, in_flight_fences_[current_frame_], "failed to picture queue submit");

        // start thread : wait render_finished_semaphores, and caculate fittness
        if (!is_top)
        {
            frame_thread_[image_index_] = std::thread(caculate_fun,
                                                      device_,
                                                      &offscreens_->get_mutable_offscreen(image_index_),
                                                      data,
                                                      &population_->get_mutable_fitness(population_idx),
                                                      &in_flight_fences_[current_frame_]);
        }
    }

    void Picture::caculate_fun(const vkcpp::Device *device,
                               vkcpp::Offscreen *offscreen,
                               const char *data,
                               double *fit,
                               VkFence *fence)
    {
        const VkFormat &image_format = offscreen->get_format();
        const VkExtent3D &extent = offscreen->get_extent();

        vkWaitForFences(*device, 1, fence, VK_TRUE, UINT64_MAX);
        const char *data2 = offscreen->map_image_memory();

        *fit = fitnessFunction(data, data2, 0, 0, extent.width, extent.height, 4, false);
    }

    void Picture::init_synobj()
    {
        image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT_);
        render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT_);
        in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT_);
        images_in_flight_.resize(offscreens_image_size_, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT_; i++)
        {
            if (vkCreateSemaphore(*device_, &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
                vkCreateSemaphore(*device_, &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
                vkCreateFence(*device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

}

inline char RGBAtoRGB(char &a, char &r)
{
    float ratio = a / 255.0f;
    r = round((1.0f - a) * r) + (a * r);
    return r;
}
/*
    using cosine similarity:  https://en.wikipedia.org/wiki/Cosine_similarity
    */
double fitnessFunction(const char *a, const char *b, int posx, int posy, int width, int height, int channel, bool is_gray)
{
    double ret = 0.0;
    double dot = 0.0, denomA = 0.0, denomB = 0.0;
    int adder = (is_gray) ? channel : 1;
    int y = posy, x = posx * channel;
    int end_y = height + y;
    int line = width * channel + (width * (4 - channel)) % 4;
    int end_x = line + x;
    const unsigned char *ua = (unsigned char *)a;
    const unsigned char *ub = (unsigned char *)b;
    //  auto new_time = std::chrono::high_resolution_clock::now();

    for (int i = y; i < end_y; i++)
    {
        for (int j = x; j < end_x; j += 4)
        {
            int bi = (i - y) * line + (j - x);
            int ai = i * line + j;
            dot += ua[ai] * ub[bi] + ua[ai + 1] * ub[bi + 1] + ua[ai + 2] * ub[bi + 2] + ua[ai + 3] * ub[bi + 3];
            denomA += ua[ai] * ua[ai] + ua[ai + 1] * ua[ai + 1] + ua[ai + 2] * ua[ai + 2] + ua[ai + 3] * ua[ai + 3];
            denomB += ub[bi] * ub[bi] + ub[bi + 1] * ub[bi + 1] + ub[bi + 2] * ub[bi + 2] + ub[bi + 3] * ub[bi + 3];

            //      std::cout << "picture r: " << (int)a[ai + 0] << "  offscreen r :" << (int)b[bi + 0] << "\n";
            //      std::cout << "picture g: " << (int)a[ai + 1] << "  offscreen g :" << (int)b[bi + 1] << "\n";
            //      std::cout << "picture b: " << (int)a[ai + 2] << "  offscreen b :" << (int)b[bi + 2] << "\n";
            //      std::cout << "picture a: " << (int)a[ai + 3] << "  offscreen a :" << (int)b[bi + 3] << "\n";
        }
    }
    /*
    auto finish_time = std::chrono::high_resolution_clock::now();
    float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(finish_time - new_time).count();
    std::cout << " calc: " << frame_time << "\n";

*/
    ret = (dot / (sqrt(denomA) * sqrt(denomB)));
    return ret;
}