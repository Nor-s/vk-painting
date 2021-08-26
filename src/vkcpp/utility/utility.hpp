#ifndef VKCPP_UTILITY_UTILITY_HPP
#define VKCPP_UTILITY_UTILITY_HPP

#include <fstream>
#include <vector>
#include <string>

namespace vkcpp
{
    std::vector<char> readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        size_t file_size = (size_t)file.tellg();
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        return buffer;
    }

} // namespace vkcpp

#endif // #ifndef VKCPP_UTILITY_UTILITY_HPP