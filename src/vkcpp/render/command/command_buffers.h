#ifndef VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H
#define VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H

#include "vulkan_header.h"
#include <vector>

namespace vkcpp
{
    class Device;
    class CommandPool;
    class RenderStage;

    class CommandBuffers
    {
    private:
        const Device *device_;

        const CommandPool *command_pool_;

        std::vector<VkCommandBuffer> handle_;

        uint32_t size_{0};

        VkCommandBufferLevel level_;

    public:
        CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level);

        ~CommandBuffers() = default;

        void init_command_buffers();

        void begin_command_buffer(int command_buffer_idx);

        void begin_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void bind_pipeline(int command_buffer_idx, const Pipeline *pipeline);

        void end_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void end_command_buffer(int command_buffer_idx);
    };

} // namespace vkcpp

#endif