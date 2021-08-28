#ifndef VKCPP_COMMAND_COMMAND_POOL_H
#define VKCPP_COMMAND_COMMAND_POOL_H

#include "vulkan_header.h"

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
    }; // class CommandPool
} // namespace vkcpp

#endif // #ifndef VKCPP_COMMAND_COMMAND_POOL_H