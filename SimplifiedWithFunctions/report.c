#include "report.h"

// External APIs
#include <vulkan/vulkan.h>

// Standard library
#include <stdio.h>
#include <stdlib.h>

void* VKAPI_PTR debug_alloc(void* pUserData, size_t size, size_t alignment,
                            VkSystemAllocationScope allocationScope) {
    *((size_t*)pUserData) += size;

    void* result = _aligned_malloc(size, alignment);
#if defined VERBOSE
    printf("Allocating 0x%X bytes to 0x%p\n", size, result);
#endif
    return result;
}

void VKAPI_PTR debug_free(void* pUserData, void* pMemory) { _aligned_free(pMemory); }

void* VKAPI_PTR debug_realloc(void* pUserData, void* pOriginal, size_t size,
                              size_t alignment,
                              VkSystemAllocationScope allocationScope) {
    *((size_t*)pUserData) += size / 2;

    void* result = _aligned_realloc(pOriginal, size, alignment);
#if defined VERBOSE
    printf("Reallocating 0x%p with 0x%X bytes to 0x%p\n", pOriginal, size, result);
#endif
    return result;
}

static size_t total_allocated = 0;

VkAllocationCallbacks callbacks = {
        .pUserData = &total_allocated,
        .pfnAllocation = debug_alloc,
        .pfnFree = debug_free,
        .pfnReallocation = debug_realloc,
};

void report(const char* function) {
    printf("Called %s allocating %zu bytes total\n", function, total_allocated);
    total_allocated = 0;
}