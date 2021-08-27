#include "framebuffers.h"

#include "device/device.h"
#include "render/swapchain/swapchain.h"
#include "render/render_pass/render_pass.h"

#include <iostream>

namespace vkcpp
{
    Framebuffers::Framebuffers(const Device *device, const RenderPass *render_pass)
        : device_(device), render_pass_(render_pass)
    {
        init_framebuffers();
    }

    Framebuffers::~Framebuffers()
    {
        destroy_framebuffers();
    }

    void Framebuffers::init_framebuffers()
    {
        const Swapchain &swapchain = render_pass_->get_ref_swapchain();

        const std::vector<VkImageView> &image_views = swapchain.get_ref_image_views();

        const VkExtent2D &swapchain_extent = swapchain.get_ref_properties().extent;

        handle_.resize(image_views.size());

        for (size_t i = 0; i < image_views.size(); i++)
        {
            VkImageView attachments[] = {image_views[i]};

            VkFramebufferCreateInfo framebufferInfo{};

            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = *render_pass_;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapchain_extent.width;
            framebufferInfo.height = swapchain_extent.height;
            framebufferInfo.layers = swapchain.get_ref_properties().array_layers; // VkswapchainCreateInfoKHR::imageArrayLayers

            if (vkCreateFramebuffer(*device_, &framebufferInfo, nullptr, &handle_[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void Framebuffers::destroy_framebuffers()
    {
        for (auto &framebuffer : handle_)
        {
            if (framebuffer != VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer(*device_, framebuffer, nullptr);
                framebuffer = VK_NULL_HANDLE;
            }
        }
    }
} // namespace vkcpp
