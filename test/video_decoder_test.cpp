/***************************************************************************
 *  This file is part of Video-Decoder.
 *
 *  Video-Decoder is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Video-Decoder is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Video-Decoder.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include <iostream>
#include <cstdio>
#include <cassert>

#include <sys/stat.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <video_decoder.hpp>

using namespace isee;
using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
  // Check argument for video file path.
  if (argc < 2) {
    printf("Usage: video_decoder_test FILE");
    return 0;
  }

  const char *video_path = argv[1];

  // Open the video with OpenCV.
  VideoCapture capture(video_path);

//  while (true) {
//    Mat frame;
//    capture >> frame;
//    if (frame.empty())
//      break;
//    imshow("Preview", frame);
//    waitKey(1);
//  }

  // Open the video file with VideoDecoder.
  FILE *video_file = fopen(video_path, "rb");
  if (!video_file) {
    fprintf(stderr, "Video file not found!");
    return -1;
  }

  // Create buffer for the video with size of the file.
  struct stat statbuf;
  stat(argv[1], &statbuf);
  __off_t video_size = statbuf.st_size;
  unsigned char *video_buf = new unsigned char[video_size];

  // Read the video into the buffer.
  size_t ret = fread(video_buf, sizeof(unsigned char), video_size, video_file);
  assert(ret == (size_t) video_size);

  // Test whether the decoder can correctly decode frames.
  int vd_frame_cnt = 0;
  VideoDecoder *decoder = new VideoDecoder();
  ret = decoder->Decode(video_buf, (size_t) video_size);
  assert(ret == DECODE_SUCCESS);
  Mat vd_frame = Mat(decoder->get_height(), decoder->get_width(), CV_8UC3);
  Mat cv_frame;
  Mat canvas;
  bool display = true;
  int frame_interval = (int) (1000 / capture.get(CV_CAP_PROP_FPS));

  bool to_cut = true;
  VideoWriter writer;
  if (to_cut)
    writer.open("cut.avi",
                CV_FOURCC('M', 'P', 'E', 'G'),
                capture.get(CV_CAP_PROP_FPS),
                Size(vd_frame.cols, vd_frame.rows));

  while (true) {
    ret = decoder->NextFrame(vd_frame.data);
    capture >> cv_frame;
    if (ret == DECODE_NO_NEXT_FRAME) {
      assert(cv_frame.empty());
      break;
    }
    assert(ret == DECODE_SUCCESS);
    assert(!vd_frame.empty());
    if (display && vd_frame_cnt >= 0) {
      resize(vd_frame, canvas, Size(vd_frame.cols >> 1, vd_frame.rows >> 1));
      imshow("VideoDecoder", canvas);
      cvMoveWindow("VideoDecoder", 300, 300);
      resize(cv_frame, canvas, Size(vd_frame.cols >> 1, vd_frame.rows >> 1));
      imshow("OpenCV", canvas);
      int key_pressed = waitKey(frame_interval);
      if ((key_pressed & ((1 << 8) - 1)) == ' ') {
        printf("Stop displaying, but continue to decode for counting frames...\n");
        destroyWindow("VideoDecoder");
        display = false;
      }
    }

    if (to_cut && vd_frame_cnt > 100 && vd_frame_cnt <= 500)
      writer << cv_frame;
    if (to_cut && vd_frame_cnt > 500) {
      cout << "Finished!" << endl;
      writer.release();
      to_cut = false;
    }

    ++vd_frame_cnt;
  }
  printf("VideoDecoder decoded %d frames.\n", vd_frame_cnt);
  destroyWindow("VideoDecoder");
  delete decoder;

  // Test memory leak.
  AVIOContextManager *manager;
  for (int i = 0; i < 10000; ++i) {
    manager = new AVIOContextManager(video_buf, (size_t) video_size, 32768000);
    delete manager;
    printf("Allocated AVIOContextManager %d times!\n", i + 1);
  }

  for (int i = 0; i < 1000; ++i) {
    decoder = new VideoDecoder();
    decoder->Decode(video_buf, (size_t) video_size);
    delete decoder;
    printf("Allocated VideoDecoder %d times!\n", i + 1);
  }

  delete[] video_buf;

  return 0;
}
