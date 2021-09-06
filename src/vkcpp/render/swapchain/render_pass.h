#ifndef VKCPP_RENDER_SWAPCHAIN_RENDER_PASS_H
#define VKCPP_RENDER_SWAPCHAIN_RENDER_PASS_H

#include "vulkan_header.h"

namespace vkcpp
{
    class Device; // for create and destroy

    class Swapchain; // for color attachment format

    class Image; // for off screen rendering

    /**
     *  A wrapper class for VkRenderPass 
     *  Specify:
     *      how many color, depth buffers and samples to use for each of them.
     *      how their contents shoud be handled throughout the rendering operations. 
     */
    class RenderPass
    {
    private:
        const Device *device_{nullptr};

        const Swapchain *swapchain_{nullptr};

        const Image *image_{nullptr};

        VkRenderPass handle_{VK_NULL_HANDLE};

    public:
        RenderPass(const Device *device, const Swapchain *swapchain);

        RenderPass(const Device *device, const Image *image);

        ~RenderPass();

        operator const VkRenderPass &() const { return handle_; }

        const Swapchain &get_swapchain() const { return *swapchain_; }

        void init_render_pass();

        void init_render_pass_for_offscreen();

        void destroy_rander_pass();
    }; // class RenderPass
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_SWAPCHAIN_RENDER_PASS_H