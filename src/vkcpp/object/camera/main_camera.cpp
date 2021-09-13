#include "main_camera.h"
#include "device/window/main_window.h"

namespace vkcpp
{
    MainCamera::MainCamera()
    {

        auto [width, height] = MainWindow::getInstance()->get_framebuffer_size();

        update_view_to_look_at(
            {(float)width / 2.0f, height / 2.0f, 100.0f},
            {0.0f, 0.0f, 200.0f},
            {0.0f, 1.0f, 0.0f});
        update_proj_to_perspective(
            glm::radians(100.0f),
            width / height,
            {0.1f, 200.0f});
        //update_proj_to_ortho({-width / 2.0f, width / 2.0f}, {-height / 2.0f, height / 2.0f}, {-100.0f, 100.0f});
        //  update_proj_to_ortho({0.0f, width}, {0.0f, height}, {-100.0f, 100.0f});
    }
    void MainCamera::update()
    {

        auto [width, height] = MainWindow::getInstance()->get_framebuffer_size();

        update_proj_to_perspective(
            glm::radians(45.0f),
            width / height,
            {0.1f, width / 2.0f + 100.0f});

        //  update_proj_to_ortho({0.0f, width}, {0.0f, height}, {-100.0f, 100.0f});
    }

} // namespace vkcpp