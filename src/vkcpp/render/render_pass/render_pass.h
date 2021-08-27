#ifndef VKCPP_RENDER_RENDERPASS_RENDER_PASS_H
#define VKCPP_RENDER_RENDERPASS_RENDER_PASS_H

#include "vulkan_header.h"

namespace vkcpp
{
    class Device;    // for create and destroy
    class Swapchain; // for color attachment format

    /**
     *  A wrapper class for VkRenderPass 
     *  Specify:
     *      how many color, depth buffers and samples to use for each of them.
     *      how their contents shoud be handled throughout the rendering operations. 
     */
    class RenderPass
    {
    private:
        const Device *device_;

        const Swapchain *swapchain_;

        VkRenderPass handle_{VK_NULL_HANDLE};

    public:
        RenderPass(const Device *device, const Swapchain *swapchain);

        ~RenderPass();

        operator const VkRenderPass &() const { return handle_; }

        void init_render_pass();

        void destroy_rander_pass();
    }; // class RenderPass
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_SWAPCHAIN_RENDER_PASS_H