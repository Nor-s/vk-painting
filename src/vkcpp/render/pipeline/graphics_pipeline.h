#ifndef VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H
#define VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H

#include "vulkan_header.h"

#include <memory>

namespace vkcpp
{
    class Swapchain;
    class RenderPass;
    class Device;
    class DescriptorSetLayout;

    class GraphicsPipeline
    {
    private:
        const Device *device_;
        const Swapchain *swapchain_;
        const RenderPass *render_pass_;
        const DescriptorSetLayout *descriptor_set_layout_;
        std::string vert_shader_file_;
        std::string frag_shader_file_;

        VkPipelineLayout layout_{VK_NULL_HANDLE};
        VkShaderModule vert_shader_module_{VK_NULL_HANDLE};
        VkShaderModule frag_shader_module_{VK_NULL_HANDLE};

        VkPipeline handle_{VK_NULL_HANDLE};

    public:
        GraphicsPipeline(const Device *device,
                         const Swapchain *swapchain,
                         const RenderPass *render_pass,
                         const DescriptorSetLayout *descriptor_set_layout,
                         std::string &vert_shader_file,
                         std::string &frag_shader_file);

        ~GraphicsPipeline();

        /**
         *  @brief create vert and frag shader module, stage create info.
         *  @return vector of vert and frag stage create info
         */
        std::vector<VkPipelineShaderStageCreateInfo> create_shader_stage_create_info_vec();

        /**
         *  @brief for Shader::Vertex2D data
         */
        VkPipelineVertexInputStateCreateInfo create_vertex_input_state_create_info();

        VkPipelineInputAssemblyStateCreateInfo create_input_assembly_state_create_info();

        /**
         *  @brief viewport size = swapchain extent, sissor = swapchain extent
         */
        VkPipelineViewportStateCreateInfo create_viewport_state_create_info();
        VkPipelineRasterizationStateCreateInfo create_rasterization_state_create_info();
        VkPipelineMultisampleStateCreateInfo create_multisample_state_create_info();
        VkPipelineColorBlendStateCreateInfo create_color_blend_state_create_info();
        void init_pipeline_layout();
        void init_pipeline();

        /**
         *  @brief Destroy pipeline and pipeline layout
         */
        void destroy();

    }; // class GraphicsPipeline
} // namespace vkcpp

#endif