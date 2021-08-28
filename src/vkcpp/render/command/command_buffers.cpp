#include "command_buffers.h"

#include "device/device.h"
#include "command_pool.h"
#include "render/render_stage.h"
#include "render/pipeline/pipeline.hpp"
#include "render/swapchain/framebuffers.h"
#include "render/swapchain/render_pass.h"
#include "render/swapchain/swapchain.h"

#include <iostream>

namespace vkcpp
{
    CommandBuffers::CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level)
        : device_(device), command_pool_(command_pool), size_(size), level_(level)
    {
        init_command_buffers();
    }

    void CommandBuffers::init_command_buffers()
    {
        handle_.resize(size_);

        VkCommandBufferAllocateInfo alloc_info{};

        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = *command_pool_;
        alloc_info.level = level_;
        alloc_info.commandBufferCount = size_;

        if (vkAllocateCommandBuffers(*device_, &alloc_info, handle_.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void CommandBuffers::begin_command_buffer(int command_buffer_idx)
    {
        VkCommandBufferBeginInfo begin_info{};

        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;                  // Optional
        begin_info.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(handle_[command_buffer_idx], &begin_info) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void CommandBuffers::begin_render_pass(int command_buffer_idx, const RenderStage *render_stage)
    {
        render_stage->begin_render_pass(handle_[command_buffer_idx], command_buffer_idx);
    }

    void CommandBuffers::bind_pipeline(int command_buffer_idx, const Pipeline *pipeline)
    {
        pipeline->bind_pipeline(handle_[command_buffer_idx]);
    }

    void CommandBuffers::end_render_pass(int command_buffer_idx, const RenderStage *render_stage)
    {
        render_stage->end_render_pass(handle_[command_buffer_idx], command_buffer_idx);
    }

    void CommandBuffers::end_command_buffer(int command_buffer_idx)
    {
        vkCmdEndRenderPass(handle_[command_buffer_idx]);
        if (vkEndCommandBuffer(handle_[command_buffer_idx]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

} // namespace vkcpp
