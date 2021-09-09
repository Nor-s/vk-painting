#include "object2d.h"

#include "model.h"
#include "device/device.h"
#include "camera/main_camera.h"
#include "render/image/image.h"
#include "render/image/image2d.h"
#include "render/render_stage.h"
#include "render/swapchain/swapchain.h"
#include "render/command/command_pool.h"
#include "render/command/command_buffers.h"
#include "render/pipeline/graphics_pipeline.h"

#include <memory>

namespace vkcpp
{
    glm::mat4 TransformComponent::get_mat4()
    {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return {
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.0f,
            },
            {translation.x, translation.y, translation.z, 1.0f}};
    }

    Object2D::Object2D(const Device *device,
                       const RenderStage *render_stage,
                       const CommandPool *command_pool,
                       const VkExtent3D &extent)
        : device_(device), render_stage_(render_stage), command_pool_(command_pool), texture_file_(nullptr)
    {
        init_texture(extent);
        init_object2d();
    }

    Object2D::Object2D(const Device *device,
                       const RenderStage *render_stage,
                       const CommandPool *command_pool,
                       const char *texture_file)
        : device_(device), render_stage_(render_stage), command_pool_(command_pool), texture_file_(texture_file)
    {
        if (texture_file != nullptr)
        {
            init_texture({});
            init_object2d();
        }
#ifdef _DEBUG__
        std::cout << "  render stage object framebufer_size_ : " << framebuffers_size_ << "\n";
#endif
    }

    Object2D::~Object2D()
    {
        destroy_dependency_renderpass();
        destroy_object2d();
    }
    const VkExtent3D &Object2D::get_extent_3d() const
    {
        return texture_->get_extent();
    }
    const GraphicsPipeline *Object2D::get_graphics_pipeline() const
    {
        return graphics_pipeline_.get();
    }
    const uint32_t Object2D::get_framebuffers_size() const
    {
        return framebuffers_size_;
    }
    UniformBuffers<shader::attribute::TransformUBO> &Object2D::get_mutable_uniform_buffers()
    {
        return *uniform_buffers_;
    }

    void Object2D::init_transform(glm::vec3 translation, glm::vec3 scale, glm::vec3 rotation)
    {
        transform_.translation = translation;
        transform_.scale = scale;
        transform_.rotation = rotation;
    }
    //TODO handle overflow
    void Object2D::add_transform(glm::vec3 translation, glm::vec3 scale, glm::vec3 rotation)
    {
        transform_.translation += translation;
        transform_.scale += scale;
        transform_.rotation += rotation;
    }

    void Object2D::update(uint32_t uniform_buffer_idx)
    {
        vkcpp::shader::attribute::TransformUBO ubo{};
        //TODO this is hard coding.
        init_transform({1, 1, 1}, {0.001f, 0.001f, 1.0f}, {0.0f, 0.0f, 0.0f});
        ubo.model = transform_.get_mat4();
        ubo.view = MainCamera::getInstance()->get_view();
        ubo.proj = MainCamera::getInstance()->get_proj();
        //ubo.proj[1][1] *= -1;
        get_mutable_uniform_buffers().update_uniform_buffer(uniform_buffer_idx, ubo);
    }

    void Object2D::init_texture(const VkExtent3D &extent)
    {
        if (texture_file_ != nullptr)
        {
            texture_ = std::make_unique<Image2D>(
                device_,
                command_pool_,
                texture_file_);
        }
        else
        {
            texture_ = std::make_unique<Image2D>(
                device_,
                command_pool_,
                extent);
        }
    }

    void Object2D::init_object2d()
    {
        load_model();
        init_dependency_renderpass(render_stage_);
    }

    void Object2D::load_model()
    {
        //       float screen_width = static_cast<float>(render_stage_->get_render_area().extent.width);

        //     float screen_height = static_cast<float>(render_stage_->get_render_area().extent.height);

        auto [width, height] = texture_->get_size();

#ifdef _DEBUG__
        //  std::cout << "       Object load_ model : " << screen_width << " ," << screen_height << "\n";
#endif

        float w = static_cast<float>(width) / 2.0f, h = static_cast<float>(height) / 2.0f;

        /*
        float screen_ratio = screen_width / screen_height, image_ratio = w / h;

        w = screen_ratio;
        h = w / image_ratio;

        if (h > 1.0f)
        {
            w /= h;
            h = 1.0f;
        }

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

    void Object2D::init_dependency_renderpass(const RenderStage *render_stage)
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

    void Object2D::destroy_dependency_renderpass()
    {
        graphics_pipeline_.reset();
        uniform_buffers_.reset();
    }

    void Object2D::destroy_object2d()
    {
        model_.reset();
        texture_.reset();
    }

    void Object2D::draw(VkCommandBuffer command_buffer, int idx)
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

    void Object2D::draw(VkCommandBuffer command_buffer, const GraphicsPipeline *graphics_pipeline, int idx)
    {
        graphics_pipeline->bind_pipeline(command_buffer);

        model_->bind(command_buffer);

        vkCmdBindDescriptorSets(
            command_buffer,
            graphics_pipeline->get_pipeline_bind_point(),
            graphics_pipeline->get_pipeline_layout(),
            0,
            1,
            &uniform_buffers_->get_sets()[idx],
            0,
            nullptr);

        model_->draw(command_buffer);
    }

    void Object2D::sub_texture(const char *path)
    {
        texture_->sub_texture_image(path);
    }

    void Object2D::sub_texture(VkImage image, VkExtent3D extent)
    {
        texture_->sub_image(image, extent, render_stage_->get_color_format());
    }
}
