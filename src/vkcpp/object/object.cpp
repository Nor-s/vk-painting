#include "object.h"

#include "model.h"
#include "device/device.h"
#include "render/image/image.h"
#include "render/render_stage.h"
#include "render/swapchain/swapchain.h"
#include "render/command/command_pool.h"
#include "render/command/command_buffers.h"
#include "render/pipeline/graphics_pipeline.h"

#include <memory>

namespace vkcpp
{
    Object::Object(const Device *device,
                   const RenderStage *render_stage,
                   const CommandPool *command_pool,
                   const char *texture_file)
        : device_(device), render_stage_(render_stage), command_pool_(command_pool), texture_file_(texture_file)
    {
        init_object();
        load_2d_model();
        init_dependency_renderpass(render_stage);
    }
    Object::~Object()
    {
        destroy_dependency_renderpass();
        destroy_object();
    }

    UniformBuffers<shader::attribute::TransformUBO> &Object::get_mutable_uniform_buffers()
    {
        return *uniform_buffers_;
    }

    void Object::init_object()
    {
        texture_ = std::make_unique<Image>(
            device_,
            command_pool_,
            texture_file_);
    }

    void Object::load_2d_model()
    {
        float screen_width = static_cast<float>(render_stage_->get_render_area().extent.width);
        float screen_height = static_cast<float>(render_stage_->get_render_area().extent.height);

        auto [width, height] = texture_->get_size();

        float w = static_cast<float>(width), h = static_cast<float>(height);
        float screen_ratio = screen_width / screen_height, image_ratio = w / h;
        w = screen_ratio;
        h = w / image_ratio;
        if (h > 1.0f)
        {
            w /= h;
            h = 1.0f;
        }
        /*
        if (image_ratio < screen_ratio)
        {
            if (w > h)
            {
                h = 1.0f / (w / h);
                w = 1.0f;
            }
            else
            {
                w = 1.0f / (h / w);
                h = 1.0f;
            }
        }
        else
        {
            if (h > w)
            {
                h = 1.0f / (h / w);
                w = 1.0f;
            }
            else
            {
                h = 1.0f / (w / h);
                w = 1.0f;
            }
        }
        */
        //interleaving vertex attributes
        std::vector<shader::attribute::Vertex> vertices = {
            {{-w, -h, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{w, -h, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{w, h, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-w, h, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

        model_ = std::make_shared<Model>(device_, command_pool_, vertices);
    }

    void Object::init_dependency_renderpass(const RenderStage *render_stage)
    {
        render_stage_ = render_stage;
        framebuffers_size_ = render_stage_->get_framebuffers().get_framebuffers_size();

        if (uniform_buffers_ != nullptr)
        {
            uniform_buffers_.reset();
        }
        uniform_buffers_ = std::make_unique<UniformBuffers<shader::attribute::TransformUBO>>(
            device_,
            texture_.get(),
            framebuffers_size_);

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

    void Object::destroy_dependency_renderpass()
    {
        graphics_pipeline_.reset();
        uniform_buffers_.reset();
    }

    void Object::destroy_object()
    {
        model_.reset();
        texture_.reset();
    }

    void Object::draw(VkCommandBuffer command_buffer, int idx)
    {
        graphics_pipeline_->bind_pipeline(command_buffer);

        model_->bind(command_buffer);

        vkCmdBindDescriptorSets(
            command_buffer,
            graphics_pipeline_->get_pipeline_bind_point(),
            graphics_pipeline_->get_pipeline_layout(),
            0,
            1,
            &uniform_buffers_->get_sets()[idx],
            0,
            nullptr);

        model_->draw(command_buffer);
    }
    void Object::sub_texture(const char *path)
    {
        texture_->sub_texture_image(path);
    }

    void Object::sub_texture(VkImage image)
    {
        texture_->sub_texture_image(image, {}, render_stage_->get_color_format());
    }

}
