#ifndef VKCPP_RENDER_SWAPCHAIN_FRAMEBUFFERS_H
#define VKCPP_RENDER_SWAPCHAIN_FRAMEBUFFERS_H

#include "vulkan_header.h"
#include <vector>

namespace vkcpp
{
    class Device;
    class Swapchain;
    class RenderPass;

    class Framebuffers
    {
    private:
        const Device *device_;

        const Swapchain *swapchain_;

        const RenderPass *render_pass_;

        std::vector<VkFramebuffer> handle_;

        uint32_t framebuffers_size_{0};

    public:
        Framebuffers(const Device *device, const RenderPass *render_pass);

        ~Framebuffers();

        const uint32_t &get_framebuffers_size() const;

        const Swapchain &get_ref_swapchain() const;

        const RenderPass &get_ref_render_pass() const;

        const std::vector<VkFramebuffer> &get_ref_handle() const;

        void init_framebuffers();

        void destroy_framebuffers();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_SWAPCHAIN_FRAMEBUFFER_H