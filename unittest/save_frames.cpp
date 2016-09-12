
/**
 * \file save_frames.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include "save_frames.h"
#include "jpeg/jpeglib.h"

bool saveJpegImage(unsigned char* raw_data, const char* save_path, int width, 
     int height, int depth, int quality) {
    /// Input checking.
    if ((width <= 0) || (height <= 0) || (depth <= 0) || (quality <= 0)) {
        fprintf(stderr, "Bad number range in saveJpegImage!\n");
        return false;
    }
    if ((raw_data == NULL) || (save_path == NULL)) {
        fprintf(stderr, "Buffer is NULL in saveJpegImage!\n");
        return false;
    }
    /// save JPEG
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE* outfile = NULL;
    JSAMPROW row_pointer[1];         /// pointer to JSAMPLE row[s]
    int      row_stride;             /// physical row width in image buffer
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    printf("%s\n", save_path);
    if ((outfile = fopen(save_path, "wb")) == NULL) {
        fprintf(stderr, "Open file error in saveJpegImage!\n");
        return false;
    }
    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = width;              /// image width and height, in pixels
    cinfo.image_height = height;
    cinfo.input_components = depth;     /// number of color components per pixel
    if (depth == 3) {
        cinfo.in_color_space = JCS_RGB;         /// colorspace of input image
    }
    else if (depth == 1) {
        cinfo.in_color_space = JCS_GRAYSCALE;
    }
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE ); /// limit to baseline-JPEG values
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * depth; /// JSAMPLEs per row in image_buffer
    while (cinfo.next_scanline < cinfo.image_height) {
        // row_pointer[0] = &raw_data[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
        row_pointer[0] = &raw_data[cinfo.next_scanline * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    //delete[] out_data;
    //out_data = NULL;

    //return SUCCESS;
    return true;
}

bool saveFrame(const VideoData& video) {
    printf("Frame width    = %d\n", video.width);
    printf("Frame height   = %d\n", video.height);
    printf("Frame channels = %d\n", video.channels);
   
    bool res = false;
    int frames_num = (int)video.frames.size();
    for (int i = 0; i < frames_num; ++i) {
        stringstream ss;
        ss << i;
        string path_temp = "output/";
        string path = path_temp + ss.str() + ".jpeg";
        res = saveJpegImage((unsigned char*)video.frames[i], path.c_str(), 
              video.width, video.height, video.channels, 80);
        if (!res) {
            return false;
        }
    }
    return true;
}
