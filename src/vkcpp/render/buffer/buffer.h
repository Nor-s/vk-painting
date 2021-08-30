#ifndef VKCPP_RENDER_BUFFER_BUFFER_H
#define VKCPP_RENDER_BUFFER_BUFFER_H

#include "vulkan_header.h"

#include "device/device.h"
#include "device/physical_device.h"
#include "render/command/command_pool.h"
#include "render/command/command_buffers.h"
#include "vertex.hpp"

#include <iostream>
#include <array>
#include <vector>

namespace vkcpp
{
    class Device;

    template <typename T>
    class Buffer
    {
    protected:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        const std::vector<T> *src_data_;

        VkBuffer handle_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

        VkBufferUsageFlagBits usage_;

    public:
        Buffer() = default;

        Buffer(const Device *device,
               const CommandPool *command_pool,
               const std::vector<T> *src_data,
               VkBufferUsageFlagBits usage,
               bool is_local);

        virtual ~Buffer();

        operator const VkBuffer &() const { return handle_; }

        virtual VkBuffer &get_mutable_buffer();

        virtual VkDeviceMemory &get_mutable_memory();

        void init_buffer(bool is_local);

        void destroy_buffer();

        void free_memory();

    private:
        CommandBuffers begin_single_time_cmd();

        void end_single_time_cmd(CommandBuffers &cmd_buffer);

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

        void copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &memory);

        void create_host_memory();

        void create_local_memory();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_BUFFER_BUFFER_H