#include "command_pool.h"

#include "device/device.h"
#include "device/physical_device.h"

#include <iostream>

namespace vkcpp
{
    CommandPool::CommandPool(const Device *device, VkCommandPoolCreateFlags flags, uint32_t queue_family_index)
    {
        init_command_pool(flags, queue_family_index);
    }

    void CommandPool::init_command_pool(VkCommandPoolCreateFlags flags, uint32_t queue_family_index)
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = queue_family_index;
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
} // namespace vkcpp
