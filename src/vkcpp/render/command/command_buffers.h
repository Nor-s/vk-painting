#ifndef VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H
#define VKCPP_RENDER_COMMAND_COMMAND_BUFFERS_H

#include "vulkan_header.h"
#include <vector>
#include <iostream>

namespace vkcpp
{
    class Device;
    class CommandPool;
    class RenderStage;
    class Pipeline;

    class CommandBuffers
    {
    public:
        static CommandBuffers beginSingleTimeCmd(const Device *device, const CommandPool *command_pool);

        static void endSingleTimeCmd(CommandBuffers &cmd_buffer);

        static void cmdCopyBuffer(VkCommandBuffer cmd_buffer,
                                  VkBuffer srcBuffer,
                                  VkBuffer dstBuffer,
                                  VkDeviceSize size);

        static void cmdSingleCopyBuffer(const Device *device,
                                        const CommandPool *command_pool,
                                        VkBuffer srcBuffer,
                                        VkBuffer dstBuffer,
                                        VkDeviceSize size);

        static void cmdCopyBufferToImage(VkCommandBuffer cmd_buffer,
                                         VkBuffer buffer,
                                         VkImage image,
                                         uint32_t width,
                                         uint32_t height);

        static void cmdImageMemoryBarrier(VkCommandBuffer cmdbuffer,
                                          VkImage image,
                                          VkAccessFlags srcAccessMask,
                                          VkAccessFlags dstAccessMask,
                                          VkImageLayout oldImageLayout,
                                          VkImageLayout newImageLayout,
                                          VkPipelineStageFlags srcStageMask,
                                          VkPipelineStageFlags dstStageMask,
                                          VkImageSubresourceRange subresourceRange);

        static void cmdCopyImage(VkCommandBuffer cmd_buffer,
                                 bool supports_blit,
                                 VkExtent3D extent,
                                 VkImageSubresourceLayers src_subresource,
                                 VkImageSubresourceLayers dst_subresource,
                                 VkImage src_image,
                                 VkImage dst_image);

    private:
        const Device *device_{nullptr};

        const CommandPool *command_pool_{nullptr};

        std::vector<VkCommandBuffer> handle_;

        VkCommandBufferLevel level_{};

    public:
        CommandBuffers(const Device *device, const CommandPool *command_pool, uint32_t size, VkCommandBufferLevel level);

        CommandBuffers(const CommandBuffers &) = delete;

        CommandBuffers(CommandBuffers &&a);

        ~CommandBuffers();

        CommandBuffers &operator=(CommandBuffers &&a);

        const VkCommandBuffer &operator[](int idx) const
        {
            if (idx < 0 || idx >= handle_.size())
            {
                throw std::runtime_error("Command Buffer index out of bound exception.");
            }
            return handle_[idx];
        }

        const Device &get_device() const { return *device_; };

        const VkCommandBuffer &get_command_buffers(int idx) const { return handle_[idx]; }

        const uint32_t size() const { return handle_.size(); }

        const std::vector<VkCommandBuffer> &get_command_buffers() const { return handle_; }

        void init_command_buffers();

        void free_command_buffers();

        void begin_command_buffer(int command_buffer_idx, VkCommandBufferUsageFlags flags);

        void begin_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void bind_pipeline(int command_buffer_idx, const Pipeline *pipeline);

        void end_render_pass(int command_buffer_idx, const RenderStage *render_stage);

        void end_command_buffer(int command_buffer_idx);

        /** 
         * submit it to a queue
	     *
	     * @param commandBuffer Command buffer to flush
	     * @param queue Queue to submit the command buffer to
	     * @param pool Command pool on which the command buffer has been created
	     * @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
	     *
	     * @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
	     * @note Uses a fence to ensure command buffer has finished executing
	     */
        void flush_command_buffer(int command_buffer_idx);
    };

} // namespace vkcpp

#endif