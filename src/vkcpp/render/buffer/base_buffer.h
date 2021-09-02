#ifndef VKCPP_RENDER_BUFFER_BASE_BUFFER_H
#define VKCPP_RENDER_BUFFER_BASE_BUFFER_H

#include "vulkan_header.h"

#include <iostream>
namespace vkcpp
{
    class CommandBuffers;

    class CommandPool;

    class Device;
    class BaseBuffer
    {
    protected:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

    public:
        BaseBuffer() = default;
        BaseBuffer(const Device *device, const CommandPool *command_pool);
        BaseBuffer(const BaseBuffer &) = delete;
        BaseBuffer(BaseBuffer &&a);
        virtual ~BaseBuffer();
        BaseBuffer &operator=(BaseBuffer &&a);

    protected:
        CommandBuffers begin_single_time_cmd();

        void end_single_time_cmd(CommandBuffers &cmd_buffer);

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

        void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &memory);
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_BUFFER_BASE_BUFFER_H