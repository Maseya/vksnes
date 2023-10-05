#include "get_memory_data.h"

// Local headers
#include "report.h"

// Standard library
#include <string.h>

VkResult get_memory_data(VkDevice device, VkDeviceMemory memory, VkDeviceSize size,
                         void* data) {
    VkResult result;

    void* mapped_memory;
    result = vkMapMemory(device, memory, 0, size, 0, &mapped_memory);
    if (result < 0) {
        return result;
    }

    report("vkMapMemory");

    memcpy(data, mapped_memory, (size_t)size);

    vkUnmapMemory(device, memory);
    report("vkUnmapMemory");

    return result;
}
