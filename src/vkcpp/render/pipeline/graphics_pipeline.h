#ifndef VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H
#define VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H

#include "vulkan_header.h"

#include <memory>

namespace vkcpp
{
    class Swapchain;
    class RenderPass;
    class Device;
    class DescriptorSet;

    class GraphicsPipeline
    {
    private:
        const Device *device_{nullptr};

        const Swapchain *swapchain_{nullptr};

        const RenderPass *render_pass_{nullptr};

        const DescriptorSet *descriptor_set_{nullptr};

        std::string vert_shader_file_;

        std::string frag_shader_file_;

        VkPipelineLayout layout_{VK_NULL_HANDLE};

        VkShaderModule vert_shader_module_{VK_NULL_HANDLE};

        VkShaderModule frag_shader_module_{VK_NULL_HANDLE};

        VkPipeline handle_{VK_NULL_HANDLE};

        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_;

        VkPipelineVertexInputStateCreateInfo vertex_input_state_{};

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_{};

        VkPipelineViewportStateCreateInfo viewport_state_{};

        VkPipelineRasterizationStateCreateInfo rasterizer_state_{};

        VkPipelineMultisampleStateCreateInfo multisample_state_{};

        VkPipelineColorBlendStateCreateInfo color_blend_state_{};

        VkPipelineDynamicStateCreateInfo dynamic_state_{};

    public:
        GraphicsPipeline(const Device *device,
                         const Swapchain *swapchain,
                         const RenderPass *render_pass,
                         const DescriptorSet *descriptor_set,
                         std::string &vert_shader_file,
                         std::string &frag_shader_file);

        ~GraphicsPipeline();

        /**
         *  @brief create vert and frag shader module, stage create info.
         *  @return vector of vert and frag stage create info
         */
        void init_shader_stage_create_info_vec();

        /**
         *  @brief for Shader::Vertex2D data
         */
        void init_vertex_input_state_create_info();

        void init_input_assembly_state_create_info();

        /**
         *  @brief viewport size = swapchain extent, sissor = swapchain extent
         */
        void init_viewport_state_create_info();

        void init_rasterization_state_create_info();

        void init_multisample_state_create_info();

        void init_color_blend_state_create_info();

        void init_dynamic_state_create_info();

        void init_pipeline_layout();

        void init_pipeline();

        /**
         *  @brief Destroy pipeline and pipeline layout
         */
        void destroy();
    }; // class GraphicsPipeline
} // namespace vkcpp

#endif