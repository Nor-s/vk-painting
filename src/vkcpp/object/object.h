#ifndef VKCPP_RENDER_OBJECT_OBJECT_H
#define VKCPP_RENDER_OBJECT_OBJECT_H

#include "vulkan_header.h"
#include "render/buffer/vertex.hpp"
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

    class Device;

    class Object
    {
    private:
        const Device *device_{nullptr};

        const RenderStage *render_stage_{nullptr};

        const CommandPool *command_pool_{nullptr};

        //interleaving vertex attributes
        std::vector<Vertex2D> vertices_ = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};

        std::vector<uint16_t> indices_ = {0, 1, 2, 2, 3, 0};

        std::string vert_shader_file_{"../shaders/vs_default.spv"};

        std::string frag_shader_file_{"../shaders/fs_default.spv"};

        std::string filename_{"../textures/img.jpg"};

        std::unique_ptr<Image> texture_;

        std::unique_ptr<UniformBuffers<TransformUBO>> uniform_buffer_;

        std::unique_ptr<Buffer<Vertex2D>> vertex_buffer_;

        std::unique_ptr<Buffer<uint16_t>> index_buffer_;

        std::unique_ptr<GraphicsPipeline> graphics_pipeline_;

        TransformUBO transformation_;

        uint32_t swapchain_image_size_{0};

    public:
        Object(const Device *device,
               const RenderStage *render_stage,
               const CommandPool *command_pool);

        virtual ~Object();

        void init_object();

        void destroy_object();

        virtual void draw(VkCommandBuffer command_buffer, int idx);

    }; // class Object
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_OBJECT_H