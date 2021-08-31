#include "buffer.h"

#include "render/command/command_pool.h"
#include <vector>

namespace vkcpp
{
    template <typename T>
    Buffer<T>::Buffer(const Device *device,
                      const CommandPool *command_pool,
                      const std::vector<T> *src_data,
                      VkBufferUsageFlagBits usage,
                      bool is_local)
        : BaseBuffer(device, command_pool_),
          src_data_(src_data),
          usage_(usage)
    {
        init_buffer(src_data, is_local);
    }

    template <typename T>
    Buffer<T>::~Buffer()
    {
        destroy_buffer();
        free_memory();
    }
    template <typename T>
    void Buffer<T>::init_buffer(bool is_local)
    {
        if (is_local)
        {
            create_local_memory();
        }
        else
        {
            create_host_memory();
        }
    }

    template <typename T>
    void Buffer<T>::destroy_buffer()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(*device_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
    }

    template <typename T>
    void Buffer<T>::free_memory()
    {
        if (memory_ != VK_NULL_HANDLE)
        {
            vkFreeMemory(*device_, memory_, nullptr);
            memory_ = VK_NULL_HANDLE;
        }
    }
    template <typename T>
    void Buffer<T>::create_host_memory()
    {
        VkDeviceSize buffer_size;
        if (src_data != nullptr)
        {
            buffer_size = sizeof(*src_data[0]) * (*src_data).size();
        }
        else // for ubo
        {
            buffer_size = sizeof(T);
        }

        create_buffer(buffer_size,
                      usage_,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      handle_,
                      memory_);

        if (src_data != nullptr)
        {
            void *dst_data;
            vkMapMemory(*device_, memory_, 0, buffer_size, 0, &dst_data);
            memcpy(dst_data, (*src_data).data(), (size_t)buffer_size);
            vkUnmapMemory(*device_, memory_);
        }
    }

    template <typename T>
    void Buffer<T>::create_local_memory()
    {
        VkDeviceSize buffer_size = sizeof(*src_data[0]) * (*src_data).size();
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      staging_buffer,
                      staging_buffer_memory);

        void *dst_data;
        vkMapMemory(*device_, staging_buffer_memory, 0, buffer_size, 0, &dst_data);
        memcpy(dst_data, (*src_data).data(), (size_t)buffer_size);
        vkUnmapMemory(*device_, staging_buffer_memory);

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage_,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      get_mutable_buffer(),
                      get_mutable_memory());

        copy_buffer(staging_buffer, get_mutable_buffer(), buffer_size);

        vkDestroyBuffer(*device_, staging_buffer, nullptr);
        vkFreeMemory(*device_, staging_buffer_memory, nullptr);
    }

} // namespace vkcpp
