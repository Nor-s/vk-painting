#ifndef CLASS_PICTURE_H
#define CLASS_PICTURE_H

#include <cstring>
#include "object/object2d.h"

namespace painting
{
    class Picture : virtual public vkcpp::Object2D
    {
    private:
    public:
        Picture(const vkcpp::Device *device,
                const vkcpp::RenderStage *render_stage,
                const vkcpp::CommandPool *command_pool,
                const VkExtent3D &extent);
    };
}

#endif
