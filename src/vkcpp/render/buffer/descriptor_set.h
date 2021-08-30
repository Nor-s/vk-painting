#ifndef VKCPP_RENDER_BUFFER_DESCRIPTOR_SET_H
#define VKCPP_RENDER_BUFFER_DESCRIPTOR_SET_H

#include "vulkan_header.h"
#include <vector>

namespace vkcpp
{
    class Device;

    class DescriptorSet
    {
    private:
        const Device *device_{nullptr};

        std::vector<VkDescriptorSetLayoutBinding> layout_bindings_;

        VkDescriptorSetLayout layout_{VK_NULL_HANDLE};

        std::vector<VkDescriptorSet> descriptor_sets_;

        VkDescriptorPool pool_{VK_NULL_HANDLE};

        uint32_t size_{};

    public:
        DescriptorSet(const Device *device, uint32_t size);

        ~DescriptorSet();

        void init_layout_bindings();

        void init_layout();

        void init_pool();

        void init_descriptor_sets();

        void destroy_layout();

        void destroy_pool();
    }; // class

} // namespace vkcpp

#endif // #define VKCPP_RENDER_BUFFER_DESCRIPTOR_SET_H