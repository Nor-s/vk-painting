#include "instance.h"

#include <iostream>
#include <cstring>

namespace vkcpp
{
    const std::vector<const char *> Instance::validation_layers_ = {
        "VK_LAYER_KHRONOS_validation"};
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
    bool Instance::check_validation_layer_support()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char *layerName : validation_layers_)
        {
            bool layer_found = false;

            for (const auto &layerProperties : available_layers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layer_found = true;
                    break;
                }
            }

            if (!layer_found)
            {
                return false;
            }
        }

        return true;
    }
    void Instance::init_instance()
    {
        /*
        if (enable_validation_layers_ && !check_validation_layer_support())
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
