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
        vkcpp::GlfwWindow::get()->set_window(width, height, title);
    }
    void PaintingApplication::init_vulkan()
    {
        //   instance_.init_instance();
    }
    void PaintingApplication::main_loop()
    {
        while (!vkcpp::GlfwWindow::get()->should_close())
        {
            vkcpp::GlfwWindow::get()->process_events();
        }
    }

    void PaintingApplication::cleanup()
    {
        vkcpp::GlfwWindow::get()->destroy_window();
    }

} // namespace painting