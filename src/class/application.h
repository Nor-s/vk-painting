#ifndef PAINTING_APPLICATION_H
#define PAINTING_APPLICATION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <memory>

#include "window/main_window.h"
#include "device/instance.h"
#include "device/device.h"
#include "device/physical_device.h"
#include "device/surface.h"

namespace painting
{
    class PaintingApplication
    {
    public:
        std::vector<const char *> device_extensions_;
        PaintingApplication();
        void run(uint32_t width = 512, uint32_t height = 512, std::string title = "painting");

    private:
        std::unique_ptr<vkcpp::Instance> instance_{nullptr};
        std::unique_ptr<vkcpp::Surface> surface_{nullptr};
        std::unique_ptr<vkcpp::Device> device_{nullptr};

        void init_window(uint32_t width, uint32_t height, std::string title);
        void init_vulkan();
        void init_instance();
        void init_surface();
        void init_device();
        /*
        void init_render_context();
        void int_gui();
        void init_state();
        void init_scene();
        */
        void main_loop();
        void cleanup();
    };
} // namespace painting
#endif // #ifndef PAINTING_APPLICATION_H