#include "TestUniformBuffers.hxx"

// Local headers
#include "Vertex.hxx"
#include "VulkanError.hxx"
#include "create_buffer.hxx"

namespace maseya::renderer {
namespace {
std::vector<vma::UniqueBuffer> create_uniform_buffers(vma::Allocator allocator,
                                                      vk::DeviceSize size,
                                                      size_t count);

vk::UniqueDescriptorPool create_descriptor_pool(vk::Device device, size_t count);
std::vector<vk::DescriptorSet> create_descriptor_sets(
        vk::Device device, vk::DescriptorPool descriptor_pool,
        vk::DescriptorSetLayout layout, const std::vector<vma::UniqueBuffer>& buffers);
}  // namespace

TestUniformBuffers::TestUniformBuffers(vk::Device device, vma::Allocator allocator,
                                       vk::DescriptorSetLayout uniform_buffer_layout,
                                       size_t count)
        : device_(device),
          allocator_(allocator),
          uniform_buffers_(create_uniform_buffers(allocator, sizeof(float), count)),
          descriptor_pool_(create_descriptor_pool(device, count)),
          descriptor_sets_(create_descriptor_sets(
                  device, *descriptor_pool_, uniform_buffer_layout, uniform_buffers_)) {
}

namespace {
std::vector<vma::UniqueBuffer> create_uniform_buffers(vma::Allocator allocator,
                                                      vk::DeviceSize size,
                                                      size_t count) {
    std::vector<vma::UniqueBuffer> result;
    for (size_t i = 0; i < count; i++) {
        result.push_back(create_buffer(allocator, size,
                                       vk::BufferUsageFlagBits::eUniformBuffer,
                                       vma::MemoryUsage::eCpuToGpu));
    }

    return result;
}

vk::UniqueDescriptorPool create_descriptor_pool(vk::Device device, size_t count) {
    // Get descriptor pool size for uniform buffer descriptors.
    vk::DescriptorPoolSize uniform_pool_size;
    uniform_pool_size.type = vk::DescriptorType::eUniformBuffer;
    uniform_pool_size.descriptorCount = static_cast<uint32_t>(count);

    // Get create info for uniform descriptor pool.
    vk::DescriptorPoolCreateInfo create_info;
    create_info.poolSizeCount = 1;
    create_info.pPoolSizes = &uniform_pool_size;
    create_info.maxSets = static_cast<uint32_t>(count);

    return device.createDescriptorPoolUnique(create_info);
}

std::vector<vk::DescriptorSet> create_descriptor_sets(
        vk::Device device, vk::DescriptorPool descriptor_pool,
        vk::DescriptorSetLayout layout, const std::vector<vma::UniqueBuffer>& buffers) {
    std::vector<vk::DescriptorSetLayout> layouts(buffers.size(), layout);

    vk::DescriptorSetAllocateInfo alloc_info;
    alloc_info.descriptorPool = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    alloc_info.pSetLayouts = layouts.data();

    std::vector<vk::DescriptorSet> descriptor_sets(layouts.size());
    vk::Result result =
            device.allocateDescriptorSets(&alloc_info, descriptor_sets.data());
    if (result != vk::Result::eSuccess) {
        throw VulkanError("Could not allocate descriptor sets for uniform buffers.");
    }

    std::vector<vk::DescriptorBufferInfo> buffer_info(buffers.size());
    std::vector<vk::WriteDescriptorSet> descriptor_write(layouts.size());
    for (size_t i = 0; i < descriptor_write.size(); i++) {
        buffer_info[i].buffer = *buffers[i];
        buffer_info[i].offset = 0;
        buffer_info[i].range = sizeof(float);

        descriptor_write[i].dstSet = descriptor_sets[i];
        descriptor_write[i].dstBinding = 0;
        descriptor_write[i].dstArrayElement = 0;
        descriptor_write[i].descriptorCount = 1;

        descriptor_write[i].descriptorType = vk::DescriptorType::eUniformBuffer;
        descriptor_write[i].pBufferInfo = &buffer_info[i];

        descriptor_write[i].pImageInfo = nullptr;
        descriptor_write[i].pTexelBufferView = nullptr;
    }

    device.updateDescriptorSets(static_cast<uint32_t>(descriptor_write.size()),
                                descriptor_write.data(), 0, nullptr);
    return descriptor_sets;
}
}  // namespace
}  // namespace maseya::renderer