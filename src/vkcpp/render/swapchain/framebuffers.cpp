#include "framebuffers.h"
#include "swapchain.h"
#include "render_pass.h"

#include "device/device.h"

#include <iostream>

namespace vkcpp
{
    Framebuffers::Framebuffers(const Device *device, const RenderPass *render_pass)
        : device_(device), render_pass_(render_pass)
    {
        swapchain_ = &(render_pass_->get_swapchain());
        init_framebuffers();
    }

    Framebuffers::~Framebuffers()
    {
        destroy_framebuffers();
    }

    void Framebuffers::init_framebuffers()
    {
        const std::vector<VkImageView> &image_views = swapchain_->get_image_views();

        const VkExtent2D &swapchain_extent = swapchain_->get_properties().extent;

        framebuffers_size_ = image_views.size();

        handle_.resize(framebuffers_size_);

        for (size_t i = 0; i < framebuffers_size_; i++)
        {
            VkImageView attachments[] = {image_views[i]};

            VkFramebufferCreateInfo framebufferInfo{};

            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = *render_pass_;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapchain_extent.width;
            framebufferInfo.height = swapchain_extent.height;
            framebufferInfo.layers = swapchain_->get_properties().array_layers; // VkswapchainCreateInfoKHR::imageArrayLayers

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
                framebuffers_size_ = 0;
            }
        }
    }
} // namespace vkcpp
