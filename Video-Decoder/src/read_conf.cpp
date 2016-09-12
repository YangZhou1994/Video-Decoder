/** \file read_conf.cpp
 */

#include <string.h>
#include "read_conf.h"
/* Read configure file. */
size_t readConfFile(const string& path, char* buffer) {
    // read parameter file
    FILE* ptr_file = NULL;
    if ((ptr_file = fopen(path.c_str(), "rb")) == NULL) {
        fprintf(stderr, "Error: open configure file FAILED! File: %s, line: %d\n",
        __FILE__, __LINE__);
        return -1;
    }
    // Get buffer length.
    fseek(ptr_file, 0L, SEEK_END);
    size_t buffer_len = ftell(ptr_file);
    rewind(ptr_file);
    // Get parameter buffer.
    if (buffer) {
        memset((char*)buffer, 0, buffer_len);
        int val = fread(buffer, 1, buffer_len, ptr_file);
    }
    fclose(ptr_file);
   
    return buffer_len; 
}
