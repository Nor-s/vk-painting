#ifndef PAINTING_APPLICATION_H
#define PAINTING_APPLICATION_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "vkcpp/window/glfw_window.h"
#include "vkcpp/device/instance.h"

namespace painting
{
    class PaintingApplication
    {
    public:
        PaintingApplication();
        void run(uint32_t width = 512, uint32_t height = 512, std::string title = "painting");

    private:
        vkcpp::Instance instance_;

        void init_window(uint32_t width, uint32_t height, std::string title);
        void init_vulkan();
        void main_loop();
        void cleanup();
    };
} // namespace painting
#endif // #ifndef PAINTING_APPLICATION_H