#ifndef PAINTING_APPLICATION_H
#define PAINTING_APPLICATION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>

#include "device/window/main_window.h"
#include "device/instance.h"
#include "device/surface.h"
#include "device/physical_device.h"
#include "device/device.h"
#include "render/render_stage.h"
#include "render/swapchain/swapchain.h"
#include "render/command/command_pool.h"
#include "object/object.h"
#include "render/command/command_buffers.h"
namespace painting
{
    class PaintingApplication
    {
        const int MAX_FRAMES_IN_FLIGHT = 3;

    public:
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        std::vector<const char *> device_extensions_ = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
#ifdef __APPLE__
            ,
            "VK_KHR_portability_subset"
#endif
        };

        PaintingApplication() = default;
        void run(uint32_t width = 512, uint32_t height = 512, std::string title = "painting");

    private:
        std::unique_ptr<vkcpp::Instance> instance_{nullptr};
        std::unique_ptr<vkcpp::Surface> surface_{nullptr};
        std::unique_ptr<vkcpp::Device> device_{nullptr};
        std::unique_ptr<vkcpp::Swapchain> swapchain_{nullptr};
        std::unique_ptr<vkcpp::RenderStage> render_stage_{nullptr};
        std::unique_ptr<vkcpp::CommandPool> command_pool_{nullptr};
        std::unique_ptr<vkcpp::Object> object_{nullptr};
        std::unique_ptr<vkcpp::CommandBuffers> command_buffers_{nullptr};

        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;
        std::vector<VkFence> images_in_flight_;
        size_t current_frame_ = 0;
        bool framebuffer_resized_ = false;

        void init_window(uint32_t width, uint32_t height, std::string title);
        void init_device();
        void init_render();
        void init_synobj();
        void init_frame();
        void update_uniform_buffer(uint32_t idx);
        void draw_frame();
        void main_loop();
        void cleanup();
        void cleanup_swapchain();
        void recreate_swapchain();
    };
} // namespace painting
#endif // #ifndef PAINTING_APPLICATION_H