#include "class/picture.h"

namespace painting
{
    Picture::Picture(const vkcpp::Device *device,
                     const vkcpp::RenderStage *render_stage,
                     const vkcpp::CommandPool *command_pool,
                     const VkExtent3D &extent)
        : Object2D(device, render_stage, command_pool, extent)
    {
        init_transform({0.0, 0.0, 0.4});
    }
}