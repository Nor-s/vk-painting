#include "buffer.h"

namespace vkcpp
{
    template <typename T>
    Buffer<T>::Buffer(const Device *device,
                      const CommandPool *command_pool,
                      const std::vector<T> *src_data,
                      VkBufferUsageFlagBits usage,
                      bool is_local)
        : device_(device),
          command_pool_(command_pool),
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
    CommandBuffers Buffer<T>::begin_single_time_cmd()
    {
        CommandBuffers cmd_buffer(device_, command_pool_, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        cmd_buffer.begin_command_buffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        return cmd_buffer;
    }

    template <typename T>
    void Buffer<T>::end_single_time_cmd(CommandBuffers &cmd_buffer)
    {
        cmd_buffer.end_command_buffer(0);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd_buffer[0];

        vkQueueSubmit(device_->get_graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(device_->get_graphics_queue());
    }

    template <typename T>
    uint32_t Buffer<T>::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
    {
        const VkPhysicalDeviceMemoryProperties &mem_properties = device_->get_gpu().get_memory_properties();

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    template <typename T>
    void Buffer<T>::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        CommandBuffers cmd_buffer = std::move(begin_single_time_cmd());

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = size;
        vkCmdCopyBuffer(cmd_buffer[0], srcBuffer, dstBuffer, 1, &copy_region);

        end_single_time_cmd(cmd_buffer);
        cmd_buffer.free_command_buffers();
    }

    template <typename T>
    void Buffer<T>::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &memory)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_info.flags = 0; // for sparse buffer memory

        if (vkCreateBuffer(*device_, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(*device_, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(*device_, &alloc_info, nullptr, &memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(*device_, buffer, memory, 0);
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
