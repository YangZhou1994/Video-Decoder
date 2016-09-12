
/** \fn main.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>
using namespace std;

#include "vpe_decoder.h"
#include "save_frames.h"

size_t getVideoStream(const string& path, char* buffer) {
    // read video file in binary type.
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
    // Get video stream.
    if (buffer) {
        memset((char*)buffer, 0, buffer_len);
        int val = fread(buffer, 1, buffer_len, ptr_file);
    }
    fclose(ptr_file);
    return buffer_len;
}

int main(int argc, char** argv) {

    // Load video stream ...
    printf("Start testing ...\n");
    string video_filename = argv[1];
    char* buffer = NULL;
    int buffer_len = getVideoStream(video_filename, buffer);
    if (buffer_len <= 0) {
        fprintf(stderr, "Error: load video stream FAILED!\n");
        return -1;
    }
    buffer = new char[buffer_len];
    buffer_len = getVideoStream(video_filename, buffer);
    printf("  ---- Load video stream SUCCESSFULLY!\n");
    // Decoding ...
    VideoData video_frames;
    bool ret = vpeDecoder(buffer_len, (unsigned char*) buffer, video_frames);
    if (!ret) {
        fprintf(stderr, "Error: decode input video stream FAILED!\n");
        return -1;
    }
    else {
        printf("  ---- Decode input video stream SUCCESSFULLY!\n");
    }

    printf("  ---- Save frames now ...\n");
    ret = saveFrame(video_frames);
    if (!ret) {
        fprintf(stderr, "Error: save video stream  into frames FAILED!\n");
        // Release.
        int frames_num = (int)video_frames.frames.size();
        for (int i = 0; i < frames_num; ++i) {
            if (video_frames.frames[i]) {
                delete[] video_frames.frames[i];
                video_frames.frames[i] = NULL;
            }
        }
        return -1;
    }
    else {
        printf("  ---- The video frame are saved frame by frame  SUCCESSFULLY!\n");
    }

    // Release.
    int frames_num = (int)video_frames.frames.size();
    for (int i = 0; i < frames_num; ++i) {
        if (video_frames.frames[i]) {
            delete[] video_frames.frames[i];
            video_frames.frames[i] = NULL;
        }
    }
    return 0;
}
