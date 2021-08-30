#ifndef VKCPP_RENDER_OBJECT_OBJECT_H
#define VKCPP_RENDER_OBJECT_OBJECT_H

#include "vulkan_header.h"

#include "render/pipeline/shader.h"
#include "render/buffer/vertex_buffer.h"
#include <vector>
#include <string>

namespace vkcpp
{
    class Object
    {
    private:
        //interleaving vertex attributes
        const std::vector<Vertex2D> vertices_ = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};
        const std::vector<uint16_t> indices_ = {0, 1, 2, 2, 3, 0};

        const std::string vert_shader_file_{"../shaders/vs_default.spv"};

        const std::string frag_shader_file_{"../shaders/fs_default.spv"};

    public:
        virtual void draw(VkCommandBuffer command_buffer);

    }; // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H