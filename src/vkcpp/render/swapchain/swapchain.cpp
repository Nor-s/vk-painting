#include "swapchain.h"

#include "window/main_window.h"
#include "device/surface.h"
#include "device/physical_device.h"
#include "device/device.h"

#include <algorithm>

namespace vkcpp
{
    Swapchain::Swapchain(const Device *device, const Surface *surface)
        : device_(device), surface_(surface)
    {
        init_swapchain(device_, surface_);
        init_images();
    }

    Swapchain::~Swapchain()
    {
        destroy_swapchain();
    }

    const std::vector<VkImage> &Swapchain::get_ref_images() const
    {
        return images_;
    }

    const std::vector<VkImageView> &Swapchain::get_ref_image_views() const
    {
        return image_views_;
    }

    const SwapchainProperties &Swapchain::get_ref_properties() const
    {
        return properties_;
    }

    VkSurfaceFormatKHR Swapchain::choose_swapchain_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats)
    {
        for (const auto &available_format : available_formats)
        {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return available_format;
            }
        }

        return available_formats[0];
    }
    VkPresentModeKHR Swapchain::choose_swapchain_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes)
    {
        for (const auto &available_present_mode : available_present_modes)
        {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return available_present_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D Swapchain::choose_swapchain_extent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            auto [width, height] = MainWindow::getInstance()->get_framebuffer_size();

            VkExtent2D actual_extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};

            actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }

    void Swapchain::init_swapchain(const Device *device, const Surface *surface)
    {
        const SwapchainSupportDetails &swapchain_support = device->get_ref_gpu().get_ref_swapchain_support();

        // Choose device surfac support detail
        properties_.surface_format = choose_swapchain_surface_format(swapchain_support.formats);
        properties_.present_mode = choose_swapchain_present_mode(swapchain_support.present_modes);
        properties_.extent = choose_swapchain_extent(swapchain_support.capabilities);

        // Decide minimum image count to have in the swap chain, +1 is spare image
        properties_.image_count = swapchain_support.capabilities.minImageCount + 1;
        if (swapchain_support.capabilities.maxImageCount > 0 && properties_.image_count > swapchain_support.capabilities.maxImageCount)
        {
            properties_.image_count = swapchain_support.capabilities.maxImageCount;
        }

        // Fill swap chain create info struct
        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = *surface;

        create_info.minImageCount = properties_.image_count;
        create_info.imageFormat = properties_.surface_format.format;
        create_info.imageColorSpace = properties_.surface_format.colorSpace;
        create_info.imageExtent = properties_.extent;
        create_info.imageArrayLayers = properties_.array_layers = 1; // for 2D
        create_info.imageUsage = properties_.image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Decide image sharing mode
        const QueueFamilyIndices &indices = device->get_ref_gpu().get_ref_queue_family_indices();
        uint32_t queueFamilyIndices[] = {indices.graphics_family.value(), indices.present_family.value()};

        if (indices.graphics_family != indices.present_family)
        {
            create_info.imageSharingMode = properties_.sharing_mode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            create_info.imageSharingMode = properties_.sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform = properties_.pre_transform = swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = properties_.composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = properties_.present_mode;
        create_info.clipped = properties_.clipped = VK_FALSE;

        create_info.oldSwapchain = properties_.old_swapchain = VK_NULL_HANDLE;

        // Create Swapchain
        if (vkCreateSwapchainKHR(*device, &create_info, nullptr, &handle_) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }
    }

    void Swapchain::destroy_swapchain()
    {
        for (auto image_view : image_views_)
        {
            vkDestroyImageView(*device_, image_view, nullptr);
        }
        if (handle_ != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(*device_, handle_, nullptr);
        }
    }

    void Swapchain::init_images()
    {
        vkGetSwapchainImagesKHR(*device_, handle_, &properties_.image_count, nullptr);
        images_.resize(properties_.image_count);
        vkGetSwapchainImagesKHR(*device_, handle_, &properties_.image_count, images_.data());
    }

    VkImageView Swapchain::create_image_view(VkImage image, VkFormat format)
    {
        // Specify createinfo.
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // dimension of texture.
        view_info.format = format;

        // components field allows to swizzle the color channels around.
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Describes what the image's purpose is and which part of the image should be accessed.
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VkImageView image_view;
        if (vkCreateImageView(*device_, &view_info, nullptr, &image_view) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return image_view;
    }

    void Swapchain::init_image_views()
    {
        image_views_.resize(images_.size());

        for (size_t i = 0; i < images_.size(); i++)
        {
            image_views_[i] = create_image_view(images_[i], properties_.surface_format.format);
        }
    }
} // namespace vkcpp