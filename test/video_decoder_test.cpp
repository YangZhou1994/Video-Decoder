#include <iostream>
#include <cstdio>

#include <sys/stat.h>

#include <opencv2/opencv.hpp>

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
	for (int i = 0; i < 100000; ++i)
	{
		manager = new AVIOContextManager(video_buf, video_size, 32768000);
		delete manager;
		printf("Allocated AVIOContextManager %d times!\n", i + 1);
	}

	for (int i = 0; i < 10000; ++i)
	{
		decoder = new VideoDecoder(video_buf, video_size);
		delete decoder;
		printf("Allocated VideoDecoder %d times!\n", i + 1);
	}

	delete[] video_buf;

	return 0;
}
