#ifndef VKCPP_DEVICE_PHYSICAL_DEVICE_H
#define VKCPP_DEVICE_PHYSICAL_DEVICE_H

#include "vulkan_header.h"

#include <optional>
#include <vector>

namespace vkcpp
{
    class Instance;
    struct QueueFamilyIndices
    {
        VkQueueFlags supported_queues = {};

        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;
        std::optional<uint32_t> transfer_family;

        bool is_graphics_and_present()
        {
            return graphics_family.has_value() && present_family.has_value();
        }
        bool is_complete()
        {
            return graphics_family.has_value() && present_family.has_value() && compute_family.has_value() && transfer_family.has_value();
        }
    };
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    /**
     *  @brief A wrapper class for VkPhysicalDevice : 
     *
     *  handling gpu features, properties, and queue families 
     *  for the logical device creation.
     */
    class PhysicalDevice
    {
    private:
        const Instance *instance_;

        VkPhysicalDevice handle_{VK_NULL_HANDLE};

        // The features that this GPU supports
        VkPhysicalDeviceFeatures supported_features_;

        // The GPU properties
        VkPhysicalDeviceProperties properties_;

        // The GPU memory properties
        VkPhysicalDeviceMemoryProperties memory_properties_;

        // The GPU queue family properties
        std::vector<VkQueueFamilyProperties> queue_family_properties_;

        std::vector<const char *> extensions_;

        QueueFamilyIndices queue_family_indices_;

        SwapchainSupportDetails swapchain_support_;

    public:
        PhysicalDevice() = default;

        PhysicalDevice(const PhysicalDevice &) = delete;

        PhysicalDevice(PhysicalDevice &&) = delete;

        PhysicalDevice(const Instance *instance, VkPhysicalDevice physical_device);

        PhysicalDevice &operator=(const PhysicalDevice &) = delete;

        PhysicalDevice &operator=(PhysicalDevice &&) = delete;

        operator const VkPhysicalDevice &() const { return handle_; }

        const Instance &get_instance() const { return *instance_; }

        const VkPhysicalDeviceProperties &get_properties() const { return properties_; }

        const VkPhysicalDeviceFeatures &get_features() const { return supported_features_; }

        const VkPhysicalDeviceMemoryProperties &get_memory_properties() const { return memory_properties_; }

        const std::vector<VkQueueFamilyProperties> &get_queue_family_properties() const { return queue_family_properties_; }

        const std::vector<const char *> &get_extensions() const { return extensions_; }

        const QueueFamilyIndices &get_queue_family_indices() const { return queue_family_indices_; }

        const SwapchainSupportDetails &get_swapchain_support() const { return swapchain_support_; }

        QueueFamilyIndices find_queue_families(VkSurfaceKHR surface);

        bool check_device_extension_support(std::vector<const char *> &requested_extensions);

        SwapchainSupportDetails query_swapchain_support(VkSurfaceKHR surface);

        bool is_device_suitable(VkSurfaceKHR surface, std::vector<const char *> &requested_extensions);
    }; // class PhysicalDevice
} // namespace vkcpp

#endif // #ifndef VKCPP_DEVICE_PHYSICAL_DEVICE_H