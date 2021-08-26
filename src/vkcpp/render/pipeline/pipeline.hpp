#ifndef VKCPP_SWAPCHAIN_PIPELINE_H
#define VKCPP_SWAPCHAIN_PIPELINE_H

#include "vulkan_header.h"

#include <memory>

namespace vkcpp
{
    class RenderPass;
    class ShaderModule;
    class Device;

    class Pipeline
    {
        /*
    private:
    public:
        /**
	     * Represents position in the render structure, first value being the renderpass and second for subpass.
	     */
        /*
        using Stage = std::pair<uint32_t, uint32_t>;

        Pipeline() = default;

        virtual ~Pipeline() = default;

        void BindPipeline(const CommandBuffer &commandBuffer) const
        {
            vkCmdBindPipeline(commandBuffer, GetPipelineBindPoint(), GetPipeline());
        }

        virtual bool IsPushDescriptors() const = 0;
        virtual const VkDescriptorSetLayout &GetDescriptorSetLayout() const = 0;
        virtual const VkDescriptorPool &get_descriptorPool() const = 0;
        virtual const VkPipeline &get_pipeline() const = 0;
        virtual const VkPipelineLayout &get_pipeline_layout() const = 0;
        virtual const VkPipelineBindPoint &GetPipelineBindPoint() const = 0;
        */
    }; // class GraphicsPipeline
} // namespace vkcpp

#endif