#ifndef VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H
#define VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H

#include "vulkan_header.h"
#include <vector>
#include <iostream>

namespace vkcpp
{
    class Device;
    class CommandPool;
    class RenderStage;
    class Pipeline;

    class CommandBuffers
    {
    private:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        std::vector<VkCommandBuffer> handle_;

        VkCommandBufferLevel level_{};

    public:
        CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level);

        CommandBuffers(const CommandBuffers &) = delete;

        CommandBuffers(CommandBuffers &&a) = default;

        ~CommandBuffers();

        CommandBuffers &operator=(CommandBuffers &&a) = default;

        const VkCommandBuffer &operator[](int idx) const
        {
            if (idx < 0 || idx >= handle_.size())
            {
                throw std::runtime_error("Command Buffer index out of bound exception.");
            }
            return handle_[idx];
        }

        const VkCommandBuffer &get_command_buffers(int idx) const { return handle_[idx]; }

        void init_command_buffers();

        void free_command_buffers();

        void begin_command_buffer(int command_buffer_idx, VkCommandBufferUsageFlags flags);

        void begin_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void bind_pipeline(int command_buffer_idx, const Pipeline *pipeline);

        void end_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void end_command_buffer(int command_buffer_idx);
    };

} // namespace vkcpp

#endif