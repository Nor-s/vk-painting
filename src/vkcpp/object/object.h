#ifndef VKCPP_RENDER_OBJECT_OBJECT_H
#define VKCPP_RENDER_OBJECT_OBJECT_H

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

    class Image;

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
        glm::mat4 mat4();

        glm::mat3 normal_matrix();
    };
    class Object
    {
    private:
        const Device *device_{nullptr};

        const RenderStage *render_stage_{nullptr};

        const CommandPool *command_pool_{nullptr};

        const char *texture_file_;

        std::string vert_shader_file_{"../shaders/vs_default.spv"};

        std::string frag_shader_file_{"../shaders/fs_default.spv"};

        std::unique_ptr<Image> texture_{nullptr};

        std::unique_ptr<UniformBuffers<shader::attribute::TransformUBO>> uniform_buffers_{nullptr};

        std::unique_ptr<GraphicsPipeline> graphics_pipeline_{nullptr};

        std::shared_ptr<Model> model_;

        uint32_t framebuffers_size_{0};

    public:
        Object(const Device *device,
               const RenderStage *render_stage,
               const CommandPool *command_pool,
               const char *texture_file);

        Object(const Object &) = delete;
        Object(Object &&) = default;
        virtual ~Object();
        Object &operator=(Object &&) = default;

        UniformBuffers<shader::attribute::TransformUBO> &get_mutable_uniform_buffers();

        void init_object();

        void init_dependency_renderpass(const RenderStage *render_stage);

        void destroy_dependency_renderpass();

        void destroy_object();

        void load_2d_model();

        virtual void draw(VkCommandBuffer command_buffer, int idx);

        void sub_texture(const char *path);

        void sub_texture(VkImage image);

    }; // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H