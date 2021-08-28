#ifndef VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H
#define VKCPP_SWAPCHAIN_PIPELINE_GRAPHICS_PIPELINE_H

#include "vulkan_header.h"
#include "pipeline.hpp"

#include <memory>

namespace vkcpp
{
    class Device;
    class RenderStage;
    class DescriptorSet;

    class GraphicsPipeline : public Pipeline
    {
    private:
        const Device *device_{nullptr};

        const RenderStage *render_stage_{nullptr};

        const DescriptorSet *descriptor_set_{nullptr};

        std::string vert_shader_file_;

        std::string frag_shader_file_;

        VkPipelineLayout layout_{VK_NULL_HANDLE};

        VkShaderModule vert_shader_module_{VK_NULL_HANDLE};

        VkShaderModule frag_shader_module_{VK_NULL_HANDLE};

        VkPipeline handle_{VK_NULL_HANDLE};

        CreateInfo info_{};

        VkPipelineBindPoint pipeline_bind_point_;

        int subpass_idx_{0};

    public:
        GraphicsPipeline(const Device *device,
                         const RenderStage *render_stage,
                         const DescriptorSet *descriptor_set,
                         std::string &vert_shader_file,
                         std::string &frag_shader_file,
                         int subpass_idx);

        virtual ~GraphicsPipeline();

        virtual const VkPipeline &get_pipeline() const override
        {
            return handle_;
        }
        virtual const VkPipelineBindPoint &get_pipeline_bind_point() const override
        {
            return pipeline_bind_point_;
        }

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