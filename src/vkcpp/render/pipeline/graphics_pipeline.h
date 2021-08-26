#ifndef VKCPP_SWAPCHAIN_PIPELINE_PIPELINE_H
#define VKCPP_SWAPCHAIN_PIPELINE_PIPELINE_H

#include "vulkan_header.h"

#include <memory>

namespace vkcpp
{
    class Swapchain;
    class RenderPass;
    class Device;

    class Pipeline
    {
    private:
        const Device *device_;
        const Swapchain *swapchain_;
        const RenderPass *render_pass_;

        VkPipelineLayout layout_;
        VkDescriptorSetLayout descriptor_set_layout_;
        VkShaderModule vert_shader_module_;
        VkShaderModule frag_shader_module_;

        VkPipeline handle_;

    public:
        Pipeline();

        void init_pipeline();

    }; // class GraphicsPipeline
} // namespace vkcpp

#endif