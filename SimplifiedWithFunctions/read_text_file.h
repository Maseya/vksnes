#ifndef READ_TEXT_FILE_H
#define READ_TEXT_FILE_H

// External APIs
#include <vulkan/vulkan.h>

VkResult read_text_file(const char* path, char** text, size_t* size);

#endif