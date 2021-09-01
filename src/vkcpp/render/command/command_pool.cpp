#include "command_pool.h"

#include "device/device.h"
#include "device/physical_device.h"

#include <iostream>

namespace vkcpp
{
    CommandPool::CommandPool(Device *device, VkCommandPoolCreateFlags flags, uint32_t queue_family_idx)
        : device_(device), queue_family_idx_(queue_family_idx)
    {
        init_command_pool(flags, queue_family_idx);
    }
    CommandPool::~CommandPool()
    {
        destroy_command_pool();
    }

    void CommandPool::init_command_pool(VkCommandPoolCreateFlags flags, uint32_t queue_family_idx)
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_idx;
        pool_info.flags = flags;

        if (vkCreateCommandPool(*device_, &pool_info, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }
    void CommandPool::destroy_command_pool()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(*device_, handle_, nullptr);
        }
    }
    void CommandPool::alloc_buffers(std::vector<VkCommandBuffer> &buffer, uint32_t size, VkCommandBufferLevel level) const
    {
        buffer.resize(size);

        VkCommandBufferAllocateInfo alloc_info{};

        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = *this;
        alloc_info.level = level;
        alloc_info.commandBufferCount = size;

        if (vkAllocateCommandBuffers(*device_, &alloc_info, buffer.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }
} // namespace vkcpp
