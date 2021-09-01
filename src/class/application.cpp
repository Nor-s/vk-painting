#include "application.h"

#include "render/buffer/buffer.h"

namespace painting
{
    void PaintingApplication::run(uint32_t width, uint32_t height, std::string title)
    {
        init_window(width, height, title);
        init_vulkan();
        main_loop();
        cleanup();
    }
    void PaintingApplication::init_window(uint32_t width, uint32_t height, std::string title)
    {
        vkcpp::MainWindow::getInstance()->set_window(width, height, title);
    }

    void PaintingApplication::init_vulkan()
    {
        std::vector<uint16_t> indices_ = {0, 1, 2, 2, 3, 0};
        init_instance();
        init_surface();
        init_device();
        swapchain_ = std::make_unique<vkcpp::Swapchain>(device_.get(), surface_.get());
        render_stage_ = std::make_unique<vkcpp::RenderStage>(device_.get(), swapchain_.get());
        command_pool_ = std::make_unique<vkcpp::CommandPool>(device_.get(), 0, device_->get_gpu().get_queue_family_indices().graphics_family.value());

        vkcpp::Buffer a(device_.get(), command_pool_.get(), &indices_, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, true);
        //  createSyncObjects();
    }
    void PaintingApplication::init_instance()
    {
        instance_ = std::make_unique<vkcpp::Instance>();
    }
    void PaintingApplication::init_surface()
    {
        VkSurfaceKHR surface = vkcpp::MainWindow::getInstance()->create_surface(*instance_);
        surface_ = std::make_unique<vkcpp::Surface>(instance_.get(), surface);
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
    void PaintingApplication::draw_frame()
    {
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
        //device wait_idel()
        //scene reset
        //state reset
        //gui reset
        //render context reset
        command_pool_.reset();
        render_stage_.reset();
        swapchain_.reset();
        device_.reset();
        surface_.reset();
        instance_.reset();
        vkcpp::MainWindow::getInstance()->destroy_window();
    }

} // namespace painting