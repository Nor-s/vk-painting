#include "base_buffer.h"
#include "render/command/command_pool.h"
#include "render/command/command_buffers.h"
#include "device/physical_device.h"

#include <iostream>

namespace vkcpp
{
    BaseBuffer::BaseBuffer(const Device *device, const CommandPool *command_pool)
        : device_(device), command_pool_(command_pool)
    {
    }

    CommandBuffers BaseBuffer::begin_single_time_cmd()
    {
        CommandBuffers cmd_buffer(device_, command_pool_, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        cmd_buffer.begin_command_buffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        return cmd_buffer;
    }

    void BaseBuffer::end_single_time_cmd(CommandBuffers &cmd_buffer)
    {
        cmd_buffer.end_command_buffer(0);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd_buffer[0];

        vkQueueSubmit(device_->get_graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(device_->get_graphics_queue());

        cmd_buffer.free_command_buffers();
    }

    uint32_t BaseBuffer::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
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

    void BaseBuffer::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        CommandBuffers cmd_buffer = std::move(begin_single_time_cmd());

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = size;
        vkCmdCopyBuffer(cmd_buffer[0], srcBuffer, dstBuffer, 1, &copy_region);

        end_single_time_cmd(cmd_buffer);
    }

    void BaseBuffer::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &memory)
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
}