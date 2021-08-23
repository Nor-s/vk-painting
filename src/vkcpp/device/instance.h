#ifndef VKCPP_INSTANCE_H
#define VKCPP_INSTANCE_H

#include <vector>

#include "vkcpp/vulkan_header.h"

namespace vkcpp
{
    class PhysicalDevice;

    class Instance
    {
    private:
        VkInstance handle_{VK_NULL_HANDLE};
        bool enable_validation_layers_ = false;

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

        void init_instance();
        void destroy_instance();
    };
} // namespace vkcpp

#endif