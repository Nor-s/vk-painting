#ifndef VKCPP_RENDER_BUFFER_BUFFER_HPP
#define VKCPP_RENDER_BUFFER_BUFFER_HPP

#include "vulkan_header.h"

#include "base_buffer.h"
#include "render/command/command_pool.h"
#include "device/device.h"

#include <vector>

namespace vkcpp
{
    template <typename T>
    class Buffer : public BaseBuffer
    {
    protected:
        std::vector<T> *src_data_;

        VkBuffer handle_{VK_NULL_HANDLE};

        VkDeviceMemory memory_{VK_NULL_HANDLE};

        VkBufferUsageFlagBits usage_;

    public:
        Buffer() = default;

        Buffer(const Device *device,
               const CommandPool *command_pool,
               std::vector<T> *src_data,
               VkBufferUsageFlagBits usage,
               bool is_local);
        Buffer(const Buffer &) = delete;
        Buffer(Buffer &&a);

        virtual ~Buffer();

        Buffer<T> &operator=(Buffer &&a);

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

#include "buffer.tpp"

#endif // #ifndef VKCPP_RENDER_BUFFER_BUFFER_HPP