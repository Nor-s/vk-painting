#ifndef VKCPP_RENDER_OBJECT_SHADER_MODULE_H
#define VKCPP_RENDER_OBJECT_SHADER_MODULE_H

#include "vulkan_header.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <string>

namespace vkcpp
{
    class Device;

    class Shader
    {
    public:
        /**
         *  create shaderModule using spirv code
         */
        static VkShaderModule createShaderModule(const Device *device, std::string &filename);
    }; // class Shader
} // namespace vkcpp

#endif // #ifndef VKCPP_RENDER_OBJECT_SHADER_MODULE_H