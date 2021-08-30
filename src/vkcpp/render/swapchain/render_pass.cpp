#include "render_pass.h"

#include "device/device.h"
#include "swapchain.h"

#include <iostream>

namespace vkcpp
{
    RenderPass::RenderPass(const Device *device, const Swapchain *swapchain)
        : device_(device), swapchain_(swapchain)
    {
        init_render_pass();
    }

    RenderPass::~RenderPass()
    {
        destroy_rander_pass();
    }

    void RenderPass::init_render_pass()
    {
        // Describe color buffer.
        VkAttachmentDescription color_attachment{};
        color_attachment.format = swapchain_->get_properties().surface_format.format;
        // The number of samples of the images.
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        // What to do with the data(color, depth) in the attachment before rendering.
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // What to do with the data(color, depth) in the attachment after rendefing.
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // For stencil current not using.
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // Image layout before renderpass begin.
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        // Image layout after renderpass begin.
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Evry subpass reference one or more of the attachment, this references are themselves structs.
        VkAttachmentReference color_attachmentRef{};
        color_attachmentRef.attachment = 0;
        // Automatically transition the attachment to this layout when the subpass is started.
        color_attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // For multiple subpasses(ex. post-processing effects).
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachmentRef;

        // Handle implicit automatically transition.
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        // Decide wait stage
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        // Decide start stage and access (write)
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // Describe renderpass create info
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &color_attachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.pDependencies = &dependency;

        // Create renderpass
        if (vkCreateRenderPass(*device_, &renderPassInfo, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void RenderPass::destroy_rander_pass()
    {
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass(*device_, handle_, nullptr);
        }
    }

} // namespace vkcpp
