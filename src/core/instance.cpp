#include "instance.h"

#include <iostream>

namespace vkcpp
{
    Instance::~Instance()
    {
        if (handle_)
        {
            destroy_instance();
        }
    }
    VkInstance Instance::get_handle()
    {
        return handle_;
    }
    bool Instance::get_enable_validation_layers()
    {
        return enable_validation_layers_;
    }
    void Instance::init_instance()
    {
        /*
        if (enable_validation_layers_ && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Painting";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enable_validation_layers_)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers_.size());
            createInfo.ppEnabledLayerNames = validation_layers_.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
        */
    }
    void Instance::destroy_instance()
    {
    }

} // namespace vkcpp
