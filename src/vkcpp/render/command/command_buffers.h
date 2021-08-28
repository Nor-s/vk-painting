#ifndef VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H
#define VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H

#include "vulkan_header.h"
#include <vector>
#include <memory>

namespace vkcpp
{
    class Device;
    class Framebuffers;
    class CommandPool;

    class CommandBuffers
    {
    private:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        std::vector<VkCommandBuffer> handle_;

        uint32_t size_{0};

    public:
        CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size);

        void init_command_buffers();
        void begin_command_buffer(int idx);
        void end_command_buffer(int idx);
    };

} // namespace vkcpp

#endif