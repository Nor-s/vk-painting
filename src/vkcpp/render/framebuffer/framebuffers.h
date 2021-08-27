#ifndef VKCPP_RENDER_FRAMEBUFFER_FRAMEBUFFERS_H
#define VKCPP_RENDER_FRAMEBUFFER_FRAMEBUFFERS_H

#include "vulkan_header.h"
#include <vector>

namespace vkcpp
{
    class RenderPass;
    class Device;
    class Framebuffers
    {
    private:
        const Device *device_;

        const RenderPass *render_pass_;

        std::vector<VkFramebuffer> handle_;

    public:
        Framebuffers(const Device *device, const RenderPass *render_pass);
        ~Framebuffers();
        void init_framebuffers();
        void destroy_framebuffers();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_FRAMEBUFFER_FRAMEBUFFER_H