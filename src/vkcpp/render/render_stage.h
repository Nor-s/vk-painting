#ifndef VKCPP_RENDER_RENDER_STAGE_H
#define VKCPP_RENDER_RENDER_STAGE_H

#include <memory>

namespace vkcpp
{
    class Swapchain;
    class Renderpass;
    class Framebuffers;

    class RenderStage
    {
    private:
        const Swapchain *swapchain_;
        std::unique_ptr<Renderpass> renderpass;
        std::unique_ptr<Framebuffers> framebuffers;

    public:
        RenderStage();
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_RENDER_STAGE_H