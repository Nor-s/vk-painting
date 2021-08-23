#ifndef VKCPP_DEVICE_INSTANCE_H
#define VKCPP_DEVICE_INSTANCE_H

#include <vector>

#include "vulkan_header.h"

namespace vkcpp
{
    class PhysicalDevice;

    class Instance
    {
    private:
        VkInstance handle_{VK_NULL_HANDLE};
        VkDebugUtilsMessengerEXT debug_messenger_{VK_NULL_HANDLE};

#ifdef NDEBUG
        const bool enable_validation_layers_ = false;
#else
        const bool enable_validation_layers_ = true;
#endif

    public:
        static const std::vector<const char *> validation_layers_;

        Instance() = default;
        Instance(const Instance &) = delete;
        Instance(Instance &&) = delete;
        ~Instance();

        Instance &operator=(const Instance &) = delete;
        Instance &operator=(Instance &&) = delete;

        VkInstance get_handle();
        bool get_enable_validation_layers();

        /**
        * query support layer 
        */
        bool check_validation_layer_support();
        /**
        * get glfw instance extensions
        */
        std::vector<const char *> get_extensions();
        /**
        * debug messenger create info 
        * : before create messenger 
        * : after destroy messenger 
        * @using instanceCreateInfo.pNext
        */
        void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT &create_info);
        void init_instance();
        void destroy_instance();
        void set_debug_messenger();

        void destroy_debug_messenger();
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);
    };
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

} // namespace vkcpp

#endif