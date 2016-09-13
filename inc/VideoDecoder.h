/** \file vpe_decoder.h 
 */

#pragma once

#ifndef _VIDEO_DECODER_H_
#define _VIDEO_DECODER_H_

#include <vector>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

#include <avio_mem_context.h>
#include <unistd.h>

using namespace std;

#define DECODE_NO_NEXT_FRAME 	0
#define DECODE_SUCCESS			1
#define DECODE_FAILURE		 	2

class VideoDecoder
{
private:
	// Variables are used by decoder.
	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx;
	SwsContext *pSWSCtx;
	AvioMemContext* pAVIOCtx;
	AVCodec *pCodec;
	int videoindex = -1;
	int numBytes = 0;
	AVFrame *pFrame, *pFrameRGB;
	uint8_t* frame;
	int index;
	AVPacket *packet;

	//frame
	int width = 0;
	int height = 0;
	int channels = 0;
public:
	inline int getWidth() const
	{
		return width;
	}
	inline int getHeight() const
	{
		return height;
	}
	inline int getChannels() const
	{
		return channels;
	}
	inline int getFrameSize() const
	{
		return getWidth() * getHeight() * getChannels();
	}

	/**
	 * @return 0: No next frame; 1: Next frame retrieved successfully; 2: Failed to decode.
	 */
	inline int nextFrame(unsigned char* frame_buf)
	{
		if (av_read_frame(pFormatCtx, packet) < 0)
			return DECODE_NO_NEXT_FRAME;

		printf("Decoding frame index: %d\n", index);
		index++;
		if (packet->stream_index == videoindex)
		{
			int got_frame = 1;
			int decoding_ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_frame, packet);
			printf("Got Frame=%d; Return value=%d\n", got_frame, decoding_ret);
			if (got_frame)
			{
				printf("D02\n");
				sws_scale(pSWSCtx, pFrame->data, pFrame->linesize, 0,
						pCodecCtx->height, pFrameRGB->data,
						pFrameRGB->linesize);
			}
			av_free_packet(packet);

			memcpy(frame_buf, pFrameRGB->data[0], getFrameSize());
			return DECODE_SUCCESS;
		}
		else
		{
			av_free_packet(packet);

			printf("Failed to decode!\n");
			return DECODE_FAILURE;
		}
	}

	inline VideoDecoder(const unsigned char *buffer, int buffer_len)
	{
		if (buffer == NULL || buffer_len == 0)
		{
			printf("Error: Can't get video stream for HDFS. \n");
			return;
		}
		av_register_all();
		avcodec_register_all();

		// Init.
		avformat_network_init();
		pFormatCtx = avformat_alloc_context();
		pAVIOCtx = new AvioMemContext((char *) buffer, buffer_len);
		printf("  ---- Hey, I am after avio_ctx()\n");
		pAVIOCtx->setAVIO();
		printf("  ---- Hey, I am after avio_ctx.setAVIO()!\n");
		pFormatCtx->pb = pAVIOCtx->getAVIO();
		printf("  ---- Hey, I am after avio_ctx.getAVIO()!\n");

		if (avformat_open_input(&pFormatCtx, NULL, NULL, NULL) != 0)
		{
			printf("Couldn't open input stream.\n");
			avformat_free_context(pFormatCtx);
			return;
		}
		if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
		{
			printf("Couldn't find stream information.\n");
			avformat_free_context(pFormatCtx);
			return;
		}
		// Obtain the video stream of the total set of streams.
		videoindex = -1;
		for (int i = 0; i < (int) (pFormatCtx->nb_streams); ++i)
		{
			if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				videoindex = i;
				break;
			}
		}
		if (videoindex == -1)
		{
			printf("Didn't find a video stream.\n");
			avformat_free_context(pFormatCtx);
			return;
		}
		pCodecCtx = pFormatCtx->streams[videoindex]->codec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if (pCodec == NULL)
		{
			printf("Codec not found.\n");
			avformat_free_context(pFormatCtx);
			return;
		}
		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
		{
			printf("Could not open codec.\n");
			avformat_free_context(pFormatCtx);
			return;
		}
		pFrame = av_frame_alloc();
		pFrameRGB = av_frame_alloc();

		numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,
				pCodecCtx->height);
		frame = (uint8_t*) av_malloc(numBytes);
		avpicture_fill((AVPicture *) pFrameRGB, frame, AV_PIX_FMT_BGR24,
				pCodecCtx->width, pCodecCtx->height);
		pSWSCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
				pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
				AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
		width = pCodecCtx->width;
		height = pCodecCtx->height;
		channels = 3;
		index = 0;
		packet = (AVPacket *) av_malloc(sizeof(AVPacket));

		cout << "Construction finished!" << endl;
	}

	//TODU: Release.
	inline ~VideoDecoder()
	{
		sws_freeContext(pSWSCtx);
		av_free(frame);
		av_frame_free(&pFrame);
		av_frame_free(&pFrameRGB);
		avcodec_close(pCodecCtx);
		avformat_close_input(&pFormatCtx);

		delete pAVIOCtx;
	}
};

#endif // _VIDEO_DECODER_H_
