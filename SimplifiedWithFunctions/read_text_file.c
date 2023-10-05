#include "read_text_file.h"

// Standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

VkResult read_text_file(const char* path, char** text, size_t* size) {
    VkResult result = VK_SUCCESS;
    char* temp = NULL;

    FILE* f = fopen(path, "r");
    if (f == NULL) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    long fsize = ftell(f);
    if (fsize == -1L) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    rewind(f);

    temp = (char*)malloc((size_t)fsize + 1);
    if (temp == NULL) {
        result = VK_ERROR_FEATURE_NOT_PRESENT;
        goto cleanup;
    }

    *size = fread(temp, 1, fsize, f);
    temp[*size] = '\0';

    *text = temp;
    temp = NULL;

cleanup:
    free(temp);

    if (f != NULL) {
        fclose(f);
    }

    return result;
}