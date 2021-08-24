#include "application.h"

namespace painting
{
    PaintingApplication::PaintingApplication() = default;
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
    }
    void PaintingApplication::init_instance()
    {
        instance_ = std::make_unique<vkcpp::Instance>();
    }
    void PaintingApplication::init_surface()
    {
        VkSurfaceKHR surface = vkcpp::MainWindow::getInstance()->create_surface(*instance_);
        surface_ = std::make_unique<vkcpp::Surface>(instance_.release(), surface);
    }
    void PaintingApplication::init_device()
    {
        /*
        auto &gpu = instance_->get_suitable_gpu(*surface_);
        gpu.set_high_priority_graphics_queue_enable(high_priority_graphics_queue);

        // Request to enable ASTC
        if (gpu.get_features().textureCompressionASTC_LDR)
        {
            gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
        }

        // Request sample required GPU features
        request_gpu_features(gpu);

        // Creating vulkan device, specifying the swapchain extension always
        if (!is_headless() || instance->is_enabled(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME))
        {
            add_device_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
        device_ = std::make_unique<vkcpp::Device>(gpu, *surface_, get_device_extensions());
        */
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
        //device_.reset();
        surface_.reset();
        instance_.reset();
        vkcpp::MainWindow::getInstance()->destroy_window();
    }

} // namespace painting