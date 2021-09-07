#include "shader_module_from_file.hxx"

// Local headers
#include "VulkanError.hxx"

// Standard library
#include <fstream>
#include <vector>

namespace maseya::renderer {
vk::UniqueShaderModule shader_module_from_file(vk::Device device,
                                               const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw VulkanError("failed to open file!");
    }

    std::vector<char> source;
    source.resize(static_cast<size_t>(file.tellg()));
    file.seekg(0);
    file.read(source.data(), static_cast<std::streamsize>(source.size()));
    file.close();

    vk::ShaderModuleCreateInfo create_info;
    create_info.codeSize = source.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(source.data());

    return device.createShaderModuleUnique(create_info);
}
}  // namespace maseya::renderer