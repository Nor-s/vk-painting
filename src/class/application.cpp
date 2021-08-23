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
        vkcpp::GlfwWindow::getInstance()->set_window(width, height, title);
    }
    void PaintingApplication::init_vulkan()
    {
        instance_.init_instance();
        instance_.set_debug_messenger();
    }
    void PaintingApplication::main_loop()
    {
        while (!vkcpp::GlfwWindow::getInstance()->should_close())
        {
            vkcpp::GlfwWindow::getInstance()->process_events();
        }
    }

    void PaintingApplication::cleanup()
    {
        instance_.destroy_debug_messenger();
        instance_.destroy_instance();
        vkcpp::GlfwWindow::getInstance()->destroy_window();
    }

} // namespace painting