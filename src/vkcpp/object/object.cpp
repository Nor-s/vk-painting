#include "object.h"

#include "render/pipeline/pipeline.hpp"

namespace vkcpp
{
    void Object::draw(VkCommandBuffer command_buffer, const Pipeline *pipeline)
    {
        VkBuffer vertex_buffers[] = {vertex_buffer_};

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers.data(), offsets);

        vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(command_buffer, pipeline->get_pipeline_bind_point(), pipeline->get_pipeline, 0, 1, &descriptorSets[i], 0, nullptr);

        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);
    }
}
