#include "application.h"

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
        init_instance();
        init_surface();
        init_device();
        swapchain_ = std::make_unique<vkcpp::Swapchain>(device_.get(), surface_.get());
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
        // Request to enable ASTC
        if (gpu.get_features().textureCompressionASTC_LDR)
        {
            gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
        }
        */

        device_ = std::make_unique<vkcpp::Device>(gpu, surface_.get());
    }
    void PaintingApplication::main_loop()
    {
        while (!vkcpp::MainWindow::getInstance()->should_close())
        {
            vkcpp::MainWindow::getInstance()->process_events();
        }
    }

    void PaintingApplication::cleanup()
    {
        //device wait_idel()
        //scene reset
        //state reset
        //gui reset
        //render context reset
        swapchain_.reset();
        device_.reset();
        surface_.reset();
        instance_.reset();
        vkcpp::MainWindow::getInstance()->destroy_window();
    }

} // namespace painting