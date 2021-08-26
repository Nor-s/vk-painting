#include "shader.h"
#include "utility/utility.hpp" // readeFile()
#include "device/device.h"

#include <string>
#include <vector>

namespace vkcpp
{
    VkShaderModule Shader::create_shader_module(const Device *device, std::string &filename)
    {
        VkShaderModule shader_module;
        std::vector<char> code = readFile(filename);
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

        if (vkCreateShaderModule(*device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shader_module;
    }
}

// namespace vkcpp
