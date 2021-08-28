#include "command_buffers.h"

#include "device/device.h"
#include "command_pool.h"
#include "render/swapchain/framebuffers.h"
#include "render/swapchain/render_pass.h"
#include "render/swapchain/swapchain.h"

#include <iostream>

namespace vkcpp
{
    CommandBuffers::CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size)
        : device_(device), command_pool_(command_pool), size_(size)
    {
        init_command_buffers();
    }

    void CommandBuffers::init_command_buffers()
    {
        handle_.resize(size_);

        VkCommandBufferAllocateInfo alloc_info{};

        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = *command_pool_;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
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

        /*

        vkCmdBindPipeline(handle_[command_buffer_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, upcast_pipeline->get_pipeline());

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(handle_[command_buffer_idx], 0, 1, vertex_buffers.data(), offsets);

        vkCmdBindIndexBuffer(handle_[command_buffer_idx], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffers[command_buffer_idx], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

 vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        */
    }

    void CommandBuffers::begin_render_pass(const RenderPass render_pass, const)
    {
        const Swapchain &swapchain = framebuffers_->get_ref_swapchain();
        // Start renderpass
        VkRenderPassBeginInfo renderPassInfo{};

        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = *render_pass;
        renderPassInfo.framebuffer = framebuffers_->get_ref_handle()[command_buffer_idx];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.get_ref_properties().extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(handle_[command_buffer_idx], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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
