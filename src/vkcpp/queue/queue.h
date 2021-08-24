#ifndef VKCPP_QUEUE_QUEUE_H
#define VKCPP_QUEUE_QUEUE_H

#include "vulkan_header.h"

namespace vkcpp
{
    /**
     *  @brief  A wrapper class for VkQueue 
     */
    class Queue
    {
    private:
        VkQueue handle_;
    }; // class Queue
} // namespace vkcpp

#endif //#ifndef VKCPP_QUEUE_QUEUE_H