#include "main_camera.h"
#include "device/window/main_window.h"

namespace vkcpp
{
    MainCamera::MainCamera()
    {
        update_view_to_look_at(
            {0.0f, 0.0f, 10.0f},
            {0.0f, 0.0f, 100.0f},
            {0.0f, 1.0f, 0.0f});
        auto [width, height] = MainWindow::getInstance()->get_framebuffer_size();
        update_proj_to_perspective(
            glm::radians(45.0f),
            width / height,
            {0.1f, 100.0f});
    }

} // namespace vkcpp