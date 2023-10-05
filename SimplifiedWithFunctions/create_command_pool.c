#include "create_command_pool.h"

// Local headers
#include "report.h"

VkResult create_command_pool(VkDevice device, uint32_t queue_family_index,
                             VkCommandPool* command_pool) {
    VkCommandPoolCreateInfo command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family_index,
    };

    return vkCreateCommandPool(device, &command_pool_create_info, &callbacks,
                               command_pool);
}