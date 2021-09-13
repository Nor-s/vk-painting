#ifndef CLASS_PICTURE_H
#define CLASS_PICTURE_H

#include <cstring>
#include <thread>
#include "object/object2d.h"
#include "render/swapchain/offscreens.h"
#include "render/render_stage.h"
#include "render/command/command_buffers.h"
#include "population.h"
#include "object/camera/sub_camera.h"

namespace painting
{
    class Picture : public vkcpp::Object2D
    {

        static const int MAX_FRAMES_IN_FLIGHT_ = 3;
        static const int MAX_THREAD_ = 20;
        std::thread frame_thread_[MAX_THREAD_];
        int thread_index_ = 0;

    public:
        static void caculate_fun(const vkcpp::Device *device,
                                 vkcpp::Offscreen *offscreen,
                                 const char *data,
                                 const char *data2,
                                 double *fit,
                                 VkFence *fence);
        void wait_thread();

    private:
        std::unique_ptr<vkcpp::Offscreens> offscreens_{};
        std::unique_ptr<vkcpp::RenderStage> offscreen_render_stage_{nullptr};
        std::unique_ptr<vkcpp::CommandBuffers> command_buffers_{};
        std::unique_ptr<Population> population_;
        std::unique_ptr<Brushes> brushes_;
        std::unique_ptr<vkcpp::SubCamera> camera_;

        std::vector<bool> is_command_buffer_updated_;

        VkExtent3D extent_;
        float width_;
        float height_;
        uint32_t offscreens_image_size_{0};

        std::vector<VkSemaphore> image_available_semaphores_;
        std::vector<VkSemaphore> render_finished_semaphores_;
        std::vector<VkFence> in_flight_fences_;
        std::vector<VkFence> images_in_flight_;
        int current_frame_{0};
        int image_index_{0};

    public:
        Picture(const vkcpp::Device *device,
                const vkcpp::CommandPool *command_pool,
                const vkcpp::RenderStage *render_stage,
                const VkExtent3D &extent,
                uint32_t swapchain_image_size,
                uint32_t population_size,
                uint32_t brush_count);
        virtual ~Picture();

        Brushes &get_mutable_brushes() { return *brushes_; }

        Population &get_mutable_population() { return *population_; }

        void run(const char *data);

        void record_command_buffers();

        void record_command_buffer(int idx);

        void init_synobj();

        void draw_frame(int population_idx, const char *data, bool is_top);
    };
}

#endif
