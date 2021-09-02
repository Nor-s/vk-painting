#include "object.h"

#include "device/device.h"
#include "render/swapchain/swapchain.h"
#include "render/command/command_pool.h"
#include "render/command/command_buffers.h"
#include "render/pipeline/graphics_pipeline.h"
#include "render/image/image.h"
#include "render/render_stage.h"

#include <memory>

namespace vkcpp
{
    Object::Object(const Device *device,
                   const RenderStage *render_stage,
                   const CommandPool *command_pool)
        : device_(device), render_stage_(render_stage), command_pool_(command_pool)
    {
        init_object();
        init_dependency_swapchain(render_stage);
    }

    Object::~Object()
    {
        destroy_dependency_swapchain();
        destroy_object();
    }

    UniformBuffers<TransformUBO> &Object::get_mutable_uniform_buffers()
    {
        return *uniform_buffers_;
    }

    void Object::init_object()
    {
        texture_ = std::make_unique<Image>(
            device_,
            command_pool_,
            filename_);

        index_buffer_ = std::make_unique<Buffer<uint16_t>>(
            device_,
            command_pool_,
            &indices_,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            true);

        vertex_buffer_ = std::make_unique<Buffer<Vertex2D>>(
            device_,
            command_pool_,
            &vertices_,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            true);
    }

    void Object::init_dependency_swapchain(const RenderStage *render_stage)
    {
        render_stage_ = render_stage;
        swapchain_image_size_ = render_stage_->get_swapchain().get_properties().image_count;

        if (uniform_buffers_ != nullptr)
        {
            uniform_buffers_.reset();
        }
        uniform_buffers_ = std::make_unique<UniformBuffers<TransformUBO>>(
            device_,
            texture_.get(),
            swapchain_image_size_);

        if (graphics_pipeline_ != nullptr)
        {
            graphics_pipeline_.reset();
        }
        graphics_pipeline_ = std::make_unique<GraphicsPipeline>(
            device_,
            render_stage_,
            uniform_buffers_.get(),
            vert_shader_file_,
            frag_shader_file_,
            0);
    }

    void Object::destroy_dependency_swapchain()
    {
        graphics_pipeline_.reset();
        uniform_buffers_.reset();
    }

    void Object::destroy_object()
    {
        vertex_buffer_.reset();
        index_buffer_.reset();
        texture_.reset();
    }

    void Object::draw(VkCommandBuffer command_buffer, int idx)
    {
        graphics_pipeline_->bind_pipeline(command_buffer);

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertex_buffer_->get_buffer(), offsets);

        vkCmdBindIndexBuffer(command_buffer, *index_buffer_, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(
            command_buffer,
            graphics_pipeline_->get_pipeline_bind_point(),
            graphics_pipeline_->get_pipeline_layout(),
            0,
            1,
            &uniform_buffers_->get_sets()[idx],
            0,
            nullptr);

        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices_.size()), 1, 0, 0, 0);
    }
}
