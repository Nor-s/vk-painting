#include "buffer.h"

#include "render/command/command_pool.h"
#include "device/device.h"

namespace vkcpp
{
    Buffer::Buffer(const Device *device,
                   const CommandPool *command_pool,
                   std::vector<uint16_t> *src_data,
                   VkBufferUsageFlagBits usage,
                   bool is_local)
        : BaseBuffer(device, command_pool),
          src_data_(src_data),
          usage_(usage)
    {
        init_buffer(is_local);
    }

    Buffer::~Buffer()
    {
        destroy_buffer();
        free_memory();
    }

    Buffer::operator const VkBuffer &() const { return handle_; }

    const VkBuffer &Buffer::get_buffer() const { return handle_; }

    VkBuffer &Buffer::get_mutable_buffer() { return handle_; }

    VkDeviceMemory &Buffer::get_mutable_memory() { return memory_; }

    void Buffer::init_buffer(bool is_local)
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

    void Buffer::destroy_buffer()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(*device_, handle_, nullptr);
            handle_ = VK_NULL_HANDLE;
        }
    }

    void Buffer::free_memory()
    {
        if (memory_ != VK_NULL_HANDLE)
        {
            vkFreeMemory(*device_, memory_, nullptr);
            memory_ = VK_NULL_HANDLE;
        }
    }
    void Buffer::create_host_memory()
    {
        VkDeviceSize buffer_size;
        if (src_data_ != nullptr)
        {
            buffer_size = sizeof((*src_data_)[0]) * (*src_data_).size();
        }
        else // for ubo
        {
            // buffer_size = sizeof(T);
        }

        create_buffer(buffer_size,
                      usage_,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      handle_,
                      memory_);

        if (src_data_ != nullptr)
        {
            void *dst_data;
            vkMapMemory(*device_, memory_, 0, buffer_size, 0, &dst_data);
            memcpy(dst_data, (*src_data_).data(), (size_t)buffer_size);
            vkUnmapMemory(*device_, memory_);
        }
    }

    void Buffer::create_local_memory()
    {
        VkDeviceSize buffer_size = sizeof((*src_data_)[0]) * (*src_data_).size();
        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      staging_buffer,
                      staging_buffer_memory);
        std::cout << "create staging \n";

        void *dst_data;
        vkMapMemory(*device_, staging_buffer_memory, 0, buffer_size, 0, &dst_data);
        memcpy(dst_data, (*src_data_).data(), (size_t)buffer_size);
        vkUnmapMemory(*device_, staging_buffer_memory);

        create_buffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage_,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                      get_mutable_buffer(),
                      get_mutable_memory());
        std::cout << "create local \n";

        copy_buffer(staging_buffer, get_mutable_buffer(), buffer_size);
        std::cout << "copy \n";

        vkDestroyBuffer(*device_, staging_buffer, nullptr);
        vkFreeMemory(*device_, staging_buffer_memory, nullptr);
    }

} // namespace vkcpp
