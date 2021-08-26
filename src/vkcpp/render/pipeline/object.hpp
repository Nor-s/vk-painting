#ifndef VKCPP_RENDER_OBJECT_OBJECT_H
#define VKCPP_RENDER_OBJECT_OBJECT_H

#include "vulkan_header.h"

#include "shader.h"
#include <vector>

namespace vkcpp
{
    class Object
    {
        class Square2D : public Object
        {
        private:
            const std::vector<Shader::Vertex> vertices = {
                {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};
            const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

        public:
        }; // class Square2D
    };     // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H