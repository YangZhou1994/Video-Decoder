/** \file vpe_decoder.h 
 */

#pragma once

#ifndef _VIDEO_DECODER_H_
#define _VIDEO_DECODER_H_

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <unistd.h>

#include <avio_context_manager.h>

namespace videodecoder
{

/**
 * The class VideoDecoder is a decoder for video stored in memory.
 */
class VideoDecoder
{
private:
	// Variables used by the decoder.
	AVFormatContext *avformat_context_;
	AVCodecContext *codec_context_;
	SwsContext *sws_context_;
	AVIOContextManager* avio_context_manager_;
	AVCodec *pCodec;
	AVFrame *frame_raw_, *frame_rgb_;
	uint8_t* frame;
	AVPacket *packet;
	int index;
	int videoindex;

	// Frame size.
	int width;
	int height;
	int channels;
public:

#define DECODE_SUCCESS			0
#define DECODE_NO_NEXT_FRAME 	1
#define DECODE_FAILURE		 	2

	inline int get_width() const
	{
		return width;
	}
	inline int get_height() const
	{
		return height;
	}
	inline int getChannels() const
	{
		return channels;
	}
	inline int getFrameSize() const
	{
		return get_width() * get_height() * getChannels();
	}

	/**
	 * Skip the next frame of the video.
	 * @return 0: Next frame retrieved successfully; -1: No next frame.
	 */
	inline int SkipFrame(int num_frames = 1)
	{
		for (int i = 0; i < num_frames; ++i)
		{
			if (av_read_frame(avformat_context_, packet) < 0)
				return DECODE_NO_NEXT_FRAME;
			index++;
		}
		return DECODE_SUCCESS;
	}

	/**
	 * Get the next frame of the video.
	 * @param frame_buf	Buffer for storing the bytes of the frame.
	 * @return 0: Next frame retrieved successfully; -1: No next frame; -2: Failed to decode.
	 */
	inline int NextFrame(unsigned char* frame_buf)
	{
		// Read a frame to the packet.
		if (av_read_frame(avformat_context_, packet) < 0)
			return DECODE_NO_NEXT_FRAME;
		index++;

		if (packet->stream_index == videoindex)
		{
			int got_picture = 1;
			int decoding_ret = avcodec_decode_video2(codec_context_, frame_raw_, &got_picture, packet);
			av_free_packet(packet);
			if (decoding_ret < 0)
				return DECODE_FAILURE;
			if (got_picture)
				sws_scale(sws_context_, frame_raw_->data, frame_raw_->linesize, 0,
						codec_context_->height, frame_rgb_->data,
						frame_rgb_->linesize);

			memcpy(frame_buf, frame_rgb_->data[0], getFrameSize());
			return DECODE_SUCCESS;
		}
		else
		{
			av_free_packet(packet);

			printf("Failed to decode!\n");
			return DECODE_FAILURE;
		}
	}

	/**
	 * Create a decoder for a particular video stored in the memory.
	 * @param video_stream	Byte stream of the video.
	 * @param buffer_len	Length of the stream.
	 */
	inline VideoDecoder(const unsigned char *video_stream, int stream_len)
	{
		av_register_all();
		avcodec_register_all();
		avformat_network_init();

		avio_context_manager_ = new AVIOContextManager(video_stream, stream_len);

		avformat_context_ = avformat_alloc_context();
		avformat_context_->pb = avio_context_manager_->getAVIOContext();

		if (avformat_open_input(&avformat_context_, NULL, NULL, NULL) != 0)
		{
			printf("Couldn't open input stream.\n");
			avformat_free_context(avformat_context_);
			return;
		}
		if (avformat_find_stream_info(avformat_context_, NULL) < 0)
		{
			printf("Couldn't find stream information.\n");
			avformat_free_context(avformat_context_);
			return;
		}

		// Obtain the video stream of the total set of streams.
		videoindex = -1;
		for (int i = 0; i < (int) (avformat_context_->nb_streams); ++i)
		{
			if (avformat_context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				videoindex = i;
				break;
			}
		}
		if (videoindex == -1)
		{
			fprintf(stderr, "Video stream not found.\n");
			avformat_free_context(avformat_context_);
			return;
		}

		codec_context_ = avformat_context_->streams[videoindex]->codec;
		pCodec = avcodec_find_decoder(codec_context_->codec_id);
		if (pCodec == NULL)
		{
			fprintf(stderr, "Codec not found.\n");
			avformat_free_context(avformat_context_);
			return;
		}
		if (avcodec_open2(codec_context_, pCodec, NULL) < 0)
		{
			fprintf(stderr, "Could not open codec.\n");
			avformat_free_context(avformat_context_);
			return;
		}
		frame_raw_ = av_frame_alloc();
		frame_rgb_ = av_frame_alloc();

		int num_bytes = avpicture_get_size(AV_PIX_FMT_BGR24, codec_context_->width, codec_context_->height);
		frame = (uint8_t*) av_malloc(num_bytes);
		avpicture_fill((AVPicture *) frame_rgb_, frame, AV_PIX_FMT_BGR24,
				codec_context_->width, codec_context_->height);
		sws_context_ = sws_getContext(codec_context_->width, codec_context_->height,
				codec_context_->pix_fmt, codec_context_->width, codec_context_->height,
				AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

		width = codec_context_->width;
		height = codec_context_->height;
		channels = 3;

		index = 0;

		packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	}

	inline ~VideoDecoder()
	{
		sws_freeContext(sws_context_);
		av_free(frame);
		av_frame_free(&frame_raw_);
		av_frame_free(&frame_rgb_);
		avcodec_close(codec_context_);
		avformat_close_input(&avformat_context_);
		avformat_free_context(avformat_context_);

		delete avio_context_manager_;
	}
};

}

#endif // _VIDEO_DECODER_H_
