#ifndef VKCPP_DEVICE_PHYSICAL_DEVICE_H
#define VKCPP_DEVICE_PHYSICAL_DEVICE_H

#include "vulkan_header.h"
/**
 * Selecting a physical device in VKinstance system
 * implicate destroyed
 */
namespace vkcpp
{
    class Instance;
    class PhysicalDevice
    {
    private:
        VkPhysicalDevice handle_{VK_NULL_HANDLE};
        const Instance *instance_;

    public:
        PhysicalDevice() = default;

        operator VkPhysicalDevice &() { return handle_; };
        //     bool is_device_suitable(VkPhysicalDevice physical_device);
        /**
         * pick available physical device
         * using
         */
        //    void init_physical_device(const Instance *instance);
    };
} // namespace vkcpp

#endif // #ifndef VKCPP_DEVICE_PHYSICAL_DEVICE_H