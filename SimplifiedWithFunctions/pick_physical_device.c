#include "pick_physical_device.h"

VkResult pick_physical_device(VkInstance instance, VkPhysicalDevice* physical_device) {
    // TODO(nrg): Actually look for a suitable device.
    uint32_t physical_device_count = 1;
    return vkEnumeratePhysicalDevices(instance, &physical_device_count,
                                      physical_device);
}