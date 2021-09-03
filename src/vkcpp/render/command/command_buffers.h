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
    public:
        static CommandBuffers beginSingleTimeCmd(const Device *device, const CommandPool *command_pool);

        static void endSingleTimeCmd(CommandBuffers &cmd_buffer);

        static void cmdCopyBuffer(const Device *device, const CommandPool *command_pool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    private:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        std::vector<VkCommandBuffer> handle_;

        VkCommandBufferLevel level_{};

    public:
        CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level);

        CommandBuffers(const CommandBuffers &) = delete;

        CommandBuffers(CommandBuffers &&a);

        ~CommandBuffers();

        CommandBuffers &operator=(CommandBuffers &&a);

        const VkCommandBuffer &operator[](int idx) const
        {
            if (idx < 0 || idx >= handle_.size())
            {
                throw std::runtime_error("Command Buffer index out of bound exception.");
            }
            return handle_[idx];
        }

        const Device &get_device() const { return *device_; };

        const VkCommandBuffer &get_command_buffers(int idx) const { return handle_[idx]; }

        const std::vector<VkCommandBuffer> &get_command_buffers() const { return handle_; }

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