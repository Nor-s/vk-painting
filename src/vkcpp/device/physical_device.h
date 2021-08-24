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
    struct SwapChainSupportDetails
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
        VkPhysicalDevice handle_{VK_NULL_HANDLE};

        const Instance *instance_;

        // The features that this GPU supports
        VkPhysicalDeviceFeatures supported_features_{};

        // The GPU properties
        VkPhysicalDeviceProperties properties_;

        // The GPU memory properties
        VkPhysicalDeviceMemoryProperties memory_properties_;

        // The GPU queue family properties
        std::vector<VkQueueFamilyProperties> queue_family_properties_;

        std::vector<const char *> extensions_;

        QueueFamilyIndices queue_family_indices_;

        SwapChainSupportDetails swap_chain_support_;

    public:
        PhysicalDevice() = default;

        PhysicalDevice(const PhysicalDevice &) = delete;

        PhysicalDevice(PhysicalDevice &&) = delete;

        PhysicalDevice(const Instance *instance, VkPhysicalDevice physical_device);

        PhysicalDevice &operator=(const PhysicalDevice &) = delete;

        PhysicalDevice &operator=(PhysicalDevice &&) = delete;

        operator const VkPhysicalDevice &() const { return handle_; };

        const VkPhysicalDeviceProperties get_properties() const;

        const VkPhysicalDeviceFeatures &get_ref_features() const;

        const VkPhysicalDeviceMemoryProperties get_memory_properties() const;

        const std::vector<VkQueueFamilyProperties> &get_ref_queue_family_properties() const;

        const std::vector<const char *> &get_ref_extensions() const;

        const QueueFamilyIndices &get_ref_queue_family_indices() const;

        const SwapChainSupportDetails &get_ref_swap_chain_support() const;

        QueueFamilyIndices find_queue_families(VkSurfaceKHR surface);

        bool check_device_extension_support(std::vector<const char *> &requested_extensions);

        SwapChainSupportDetails query_swap_chain_support(VkSurfaceKHR surface);

        bool is_device_suitable(VkSurfaceKHR surface, std::vector<const char *> &requested_extensions);
    }; // class PhysicalDevice
} // namespace vkcpp

#endif // #ifndef VKCPP_DEVICE_PHYSICAL_DEVICE_H