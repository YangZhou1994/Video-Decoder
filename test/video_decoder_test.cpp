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

using namespace videodecoder;
using namespace cv;

int main(int argc, char* argv[])
{
	// Check argument for video file path.
	if (argc < 2)
	{
		printf("Usage: video_decoder_test FILE");
		return 0;
	}

	// Open video file.
	FILE* video_file = fopen(argv[1], "rb");
	if (!video_file)
	{
		fprintf(stderr, "Video file not found!");
		return -1;
	}

	// Create buffer for the video with size of the file.
	struct stat statbuf;
	stat(argv[1], &statbuf);
	__off_t video_size = statbuf.st_size;
	unsigned char* video_buf = new unsigned char[video_size];

	// Read the video into the buffer.
	size_t ret = fread(video_buf, sizeof(unsigned char), video_size, video_file);
	assert(ret == (size_t) video_size);

	// Test whether the decoder can correctly decode frames.
	int frame_cnt = 0;
	VideoDecoder* decoder = new VideoDecoder(video_buf, video_size);
	Mat frame(decoder->get_height(), decoder->get_width(), CV_8UC3);
	while (!decoder->NextFrame(frame.data))
	{
		imshow("Video", frame);
		if (waitKey(10) == 'c')
			break;
		++frame_cnt;
		printf("Decoded frame %d!\n", frame_cnt);
	}
	destroyWindow("Video");
	delete decoder;

	// Test memory leak.
	AVIOContextManager* manager;
	for (int i = 0; i < 10000; ++i)
	{
		manager = new AVIOContextManager(video_buf, video_size, 32768000);
		delete manager;
		printf("Allocated AVIOContextManager %d times!\n", i + 1);
	}

	for (int i = 0; i < 1000; ++i)
	{
		decoder = new VideoDecoder(video_buf, video_size);
		delete decoder;
		printf("Allocated VideoDecoder %d times!\n", i + 1);
	}

	delete[] video_buf;

	return 0;
}
