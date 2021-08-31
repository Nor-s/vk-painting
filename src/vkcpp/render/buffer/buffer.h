#ifndef VKCPP_RENDER_BUFFER_BUFFER_H
#define VKCPP_RENDER_BUFFER_BUFFER_H

#include "vulkan_header.h"

#include "base_buffer.h"

#include <iostream>
#include <array>
#include <vector>

namespace vkcpp
{
    class CommandPool;
    template <typename T>
    class Buffer : public BaseBuffer
    {
    protected:
        const std::vector<T> *src_data_;

        VkBuffer handle_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

        VkBufferUsageFlagBits usage_;

    public:
        //Buffer() = default;

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

        void create_host_memory();

        void create_local_memory();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_BUFFER_BUFFER_H