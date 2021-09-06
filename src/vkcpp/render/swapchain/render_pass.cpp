#include "render_pass.h"

#include "device/device.h"
#include "swapchain.h"
#include "render/image/image.h"

#include <iostream>

namespace vkcpp
{
    RenderPass::RenderPass(const Device *device, const Swapchain *swapchain)
        : device_(device), swapchain_(swapchain), image_(nullptr)
    {
        init_render_pass();
    }
    RenderPass::RenderPass(const Device *device, const Image *image)
        : device_(device), swapchain_(nullptr), image_(image)
    {
        init_render_pass_for_offscreen();
    }

    RenderPass::~RenderPass()
    {
        destroy_rander_pass();
    }

    void RenderPass::init_render_pass()
    {
        if (swapchain_ == nullptr)
        {
            throw std::runtime_error("failed to init renderpass for offscreen!");
        }
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
        dependency.srcAccessMask = 0; // VK_ACCESS_MEMORY_READ_BIT
        // Decide start stage and access (write)
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT

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

namespace vkcpp
{
    void RenderPass::init_render_pass_for_offscreen()
    {
        if (image_ == nullptr)
        {
            throw std::runtime_error("failed to init renderpass for offscreen!");
        }
        auto color_format = image_->get_color_format();
        auto depth_format = image_->get_depth_format();
        // Create a separate render pass for the offscreen rendering as it may differ from the one used for scene rendering
        std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
        // Color attachment
        attchmentDescriptions[0].format = color_format;
        attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        // Depth attachment
        attchmentDescriptions[1].format = depth_format;
        attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

        attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorReference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkAttachmentReference depthReference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorReference;
        subpassDescription.pDepthStencilAttachment = &depthReference;

        // Use subpass dependencies for layout transitions
        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        // Create the actual renderpass
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
        renderPassInfo.pAttachments = attchmentDescriptions.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpassDescription;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(*device_, &renderPassInfo, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass for offscreen rendering!");
        }
    }

} // namespace vkcpp
