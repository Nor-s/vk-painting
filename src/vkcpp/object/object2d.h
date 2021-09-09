#ifndef VKCPP_RENDER_OBJECT_OBJECT_2D_H
#define VKCPP_RENDER_OBJECT_OBJECT_2D_H

#include "vulkan_header.h"
#include "shader_attribute.hpp"
#include "render/buffer/uniform_buffers.hpp"
#include <vector>
#include <string>
#include <memory>

namespace vkcpp
{
    class GraphicsPipeline;

    class RenderStage;

    class Image2D;

    class CommandPool;

    class CommandBuffers;

    class Model;

    class Device;

    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};
        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 get_mat4();
    };

    class Object2D
    {
    protected:
        const Device *device_{nullptr};

        const RenderStage *render_stage_{nullptr};

        const CommandPool *command_pool_{nullptr};

        const char *texture_file_{nullptr};

        std::string vert_shader_file_{"../shaders/vs_default.spv"};

        std::string frag_shader_file_{"../shaders/fs_default.spv"};

        std::unique_ptr<Image2D> texture_{nullptr};

        std::unique_ptr<UniformBuffers<shader::attribute::TransformUBO>> uniform_buffers_{nullptr};

        std::unique_ptr<GraphicsPipeline> graphics_pipeline_{nullptr};

        TransformComponent transform_{};

        std::shared_ptr<Model> model_{nullptr};

        uint32_t framebuffers_size_{0};

    public:
        Object2D() = delete;

        /**
         *  for bin texture (255 255 255 ...)
         */
        Object2D(const Device *device,
                 const RenderStage *render_stage,
                 const CommandPool *command_pool,
                 const VkExtent3D &extent);

        /**
        *   if texture_file == nullptr, then init is not exe.
        */
        Object2D(const Device *device,
                 const RenderStage *render_stage,
                 const CommandPool *command_pool,
                 const char *texture_file = nullptr);

        Object2D(const Object2D &) = delete;

        Object2D(Object2D &&) = default;

        virtual ~Object2D();

        Object2D &operator=(Object2D &&) = default;

        const GraphicsPipeline *get_graphics_pipeline() const;

        const uint32_t get_framebuffers_size() const;

        const VkExtent3D &get_extent_3d() const;

        UniformBuffers<shader::attribute::TransformUBO> &get_mutable_uniform_buffers();

        void init_transform(glm::vec3 translation, glm::vec3 scale = {1.0f, 1.0f, 1.0f}, glm::vec3 rotation = {});

        void add_transform(glm::vec3 translation, glm::vec3 scale = {}, glm::vec3 rotation = {});

        void update(uint32_t uniform_buffer_idx);

        void init_texture(const VkExtent3D &extent);

        void init_object2d();

        void init_dependency_renderpass(const RenderStage *render_stage);

        void destroy_dependency_renderpass();

        void destroy_object2d();

        void load_model();

        virtual void draw(VkCommandBuffer command_buffer, int idx);

        virtual void draw(VkCommandBuffer command_buffer, const GraphicsPipeline *graphics_pipeline, int idx);

        void sub_texture(const char *path);

        void sub_texture(VkImage image, VkExtent3D extent);

    }; // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H