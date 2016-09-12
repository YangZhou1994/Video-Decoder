/** \file vpe_decoder.cpp
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>
using namespace std;

#include <avio_mem_context.h>
#include <decoder.h>

#include <unistd.h>

bool decode(int buffer_len, unsigned char* buffer, VideoData& data)
{
	if ((buffer_len <= 0) || (buffer == NULL))
		return false;

	av_register_all();
	avcodec_register_all();

	// Variables are used by decoder.
	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx;
	SwsContext *pSWSCtx;
	AVCodec *pCodec;
	int i, videoindex;
	int numBytes, frameFinished;

	// Init.
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	AvioMemContext avio_ctx((char *) buffer, buffer_len);
	printf("  ---- Hey, I am after avio_ctx()\n");
	avio_ctx.setAVIO();
	printf("  ---- Hey, I am after avio_ctx.setAVIO()!\n");
	pFormatCtx->pb = avio_ctx.getAVIO();
	printf("  ---- Hey, I am after avio_ctx.getAVIO()!\n");

	if (avformat_open_input(&pFormatCtx, NULL, NULL, NULL) != 0)
	{
		printf("Couldn't open input stream.\n");
		avformat_free_context(pFormatCtx);
		return false;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		printf("Couldn't find stream information.\n");
		avformat_free_context(pFormatCtx);
		return false;
	}

	// Obtain the video stream of the total set of streams.
	videoindex = -1;
	for (i = 0; i < (int) (pFormatCtx->nb_streams); ++i)
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
		return false;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		avformat_free_context(pFormatCtx);
		return false;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Could not open codec.\n");
		avformat_free_context(pFormatCtx);
		return false;
	}

	AVFrame *pFrame, *pFrameRGB;
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width,
			pCodecCtx->height);
	uint8_t* frame = (uint8_t*) av_malloc(numBytes);
	avpicture_fill((AVPicture *) pFrameRGB, frame, AV_PIX_FMT_BGR24,
			pCodecCtx->width, pCodecCtx->height);
	pSWSCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
			AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	data = VideoData(pCodecCtx->width, pCodecCtx->height, 3);

	//jlong pointer = 1;
//	VideoData* p = new VideoData(pCodecCtx->width, pCodecCtx->height, 3);

	//p = (VideoData*)pointer;
	//p->addFrame(frame);
	//free(p);

	i = 0;
	int index = 0;
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	while (av_read_frame(pFormatCtx, packet) >= 0)
	{
		printf("  Decoding frame index: %d\n", index);
		index++;
		if (packet->stream_index == videoindex)
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);
			if (frameFinished)
			{
				sws_scale(pSWSCtx, pFrame->data, pFrame->linesize, 0,
						pCodecCtx->height, pFrameRGB->data,
						pFrameRGB->linesize);
				data.addFrame(pFrameRGB->data[0]);
				printf("  ==== i: %d\n", i);
				i++;
			}
		}
		av_free_packet(packet);
	}

	// Release.
	sws_freeContext(pSWSCtx);
	av_free(frame);
	av_frame_free(&pFrame);
	av_frame_free(&pFrameRGB);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return true;
}
