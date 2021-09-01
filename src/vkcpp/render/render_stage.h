#ifndef VKCPP_RENDER_RENDER_STAGE_H
#define VKCPP_RENDER_RENDER_STAGE_H

#include "render/swapchain/render_pass.h"
#include "render/swapchain/framebuffers.h"
#include <memory>
#include <vector>

namespace vkcpp
{
    class Swapchain;

    class RenderStage
    {
    private:
        const Device *device_;
        const Swapchain *swapchain_;

        std::unique_ptr<RenderPass> render_pass_;

        std::unique_ptr<Framebuffers> framebuffers_;

        VkRect2D render_area_{};

        std::vector<VkClearValue> clear_values_;

    public:
        RenderStage(const Device *deivce, const Swapchain *swapchain);

        virtual ~RenderStage();

        const Swapchain &get_swapchain() const { return *swapchain_; }

        const VkRect2D &get_render_area() const { return render_area_; }

        const RenderPass &get_render_pass() const { return *render_pass_; }

        const Framebuffers &get_framebuffers() const { return *framebuffers_; }

        const VkFramebuffer &get_framebuffer(int idx) const { return framebuffers_->get_framebuffers()[idx]; }

        const std::vector<VkClearValue> get_clear_values() const { return clear_values_; }

        void init_render_stage();

        void destroy();

        void begin_render_pass(const VkCommandBuffer &command_buffer, int framebuffer_idx) const;

        void end_render_pass(const VkCommandBuffer &command_buffer, int framebuffer_idx) const;
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_RENDER_STAGE_H