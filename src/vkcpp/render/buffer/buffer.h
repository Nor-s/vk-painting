#ifndef VKCPP_RENDER_BUFFER_BUFFER_H
#define VKCPP_RENDER_BUFFER_BUFFER_H

#include "vulkan_header.h"

#include "base_buffer.h"

#include <vector>

namespace vkcpp
{
    class Buffer : public BaseBuffer
    {
    protected:
        std::vector<uint16_t> *src_data_;

        VkBuffer handle_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

        VkBufferUsageFlagBits usage_;

    public:
        Buffer() = default;

        Buffer(const Device *device,
               const CommandPool *command_pool,
               std::vector<uint16_t> *src_data,
               VkBufferUsageFlagBits usage,
               bool is_local);

        virtual ~Buffer();

        operator const VkBuffer &() const;

        const VkBuffer &get_buffer() const;

        VkBuffer &get_mutable_buffer();

        VkDeviceMemory &get_mutable_memory();

        void init_buffer(bool is_local);

        void destroy_buffer();

        void free_memory();

        void create_host_memory();

        void create_local_memory();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_BUFFER_BUFFER_H