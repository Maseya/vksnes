#ifndef REPORT_H
#define REPORT_H

// External APIs
#include <vulkan/vulkan.h>

extern VkAllocationCallbacks callbacks;

void report(const char* function);

#endif