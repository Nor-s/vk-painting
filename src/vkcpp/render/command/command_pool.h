#ifndef VKCPP_COMMAND_COMMAND_POOL_H
#define VKCPP_COMMAND_COMMAND_POOL_H

#include "vulkan_header.h"
#include <vector>
namespace vkcpp
{
    class Device;

    class CommandPool
    {
    private:
        const Device *device_;

        VkCommandPool handle_;

        uint32_t queue_family_idx_;

    public:
        CommandPool(const Device *device, VkCommandPoolCreateFlags flags, uint32_t queue_family_index);

        operator const VkCommandPool &() const { return handle_; }

        void init_command_pool(VkCommandPoolCreateFlags flags, uint32_t queue_family_index);

        void destroy_command_pool();

        void alloc_buffers(std::vector<VkCommandBuffer> &buffer, uint32_t size, VkCommandBufferLevel level) const;
    }; // class CommandPool
} // namespace vkcpp

#endif // #ifndef VKCPP_COMMAND_COMMAND_POOL_H