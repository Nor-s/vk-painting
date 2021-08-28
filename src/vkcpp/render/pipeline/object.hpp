#ifndef VKCPP_RENDER_OBJECT_OBJECT_H
#define VKCPP_RENDER_OBJECT_OBJECT_H

#include "vulkan_header.h"

#include "shader.h"
#include <vector>

namespace vkcpp
{
    class Object
    {
        class Square2D
        {
            friend Object;

        private:
            const std::vector<Shader::Vertex2D> vertices = {
                {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
                {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};
            const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

        public:
            /*
draw
        vkCmdBindPipeline(handle_[command_buffer_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, upcast_pipeline->get_pipeline());

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(handle_[command_buffer_idx], 0, 1, vertex_buffers.data(), offsets);

        vkCmdBindIndexBuffer(handle_[command_buffer_idx], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffers[command_buffer_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

 vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        */

        }; // class Square2D
    };     // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H