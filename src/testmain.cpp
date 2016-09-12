/** \file  main.cpp
 *  \brief It is test stub (decode the video with opencv here).
 *  \email da.li@cripac.ia.ac.cn
 */

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <vector>
using namespace std;

#include "tracking_intf.h"

// Test stub
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "read_conf.h"
// =========

int main(int argc, char* argv[]) {
    // The path of config file.
    string conf_path = argv[1];
    // The video path.
    string video_path= argv[2];

    // Read the buffer of configure file ... (only a test stub)
    char* buffer = NULL;
    size_t buffer_len = readConfFile(conf_path, buffer);
    buffer = new char[buffer_len];
    buffer_len = readConfFile(conf_path, buffer);
    // ...
    // Video decoder ... (only a test stub)
    CvCapture* capture = NULL;
    capture = cvCaptureFromFile(video_path.c_str());

    if (capture == NULL) {
        fprintf(stderr, "Error when read video: %s\n", video_path.c_str());
        return -1;
    }
    // ...
    int w = 1280;
    int h = 720;
    int chns_num = 3;
    
    int frame_idx = 0;

    ObjTracking tracker;
    // Init ...
    bool res = tracker.init(w, h, chns_num, buffer, buffer_len);
    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }
    if (!res) {
        fprintf(stderr, "Error: in test stub, the tracker initialization FAILED!\n");
        return -1;
    }
    // ...
    IplImage* frame = NULL;
    //cvNamedWindow("x");
    while ((frame = cvQueryFrame(capture)) != NULL) {
        frame_idx++;
        // Tracking ...
        res = tracker.doTrack((const unsigned char*)(frame->imageData));
        if (!res) {
            fprintf(stderr, "Error: when process frame %d!\n", frame_idx);
            break;
        }
        printf("frame_idx = %d\n", frame_idx);
        //cvShowImage("x", frame);
        //cvWaitKey(5);
        // ...
    }
    //cvDestroyWindow("x");
    cvReleaseCapture(&capture);
    capture = NULL;

    // Get the results ...
    TrackList tracks;
    int tracks_num = tracker.getTargets(tracks);
    printf("tracks_num = %d\n", tracks_num);
    // ...
    // Release ...
    tracker.destroy();
    // ...
    
    return 0;
}
