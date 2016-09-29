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

namespace isee {

/**
 * @class VideoDecoder
 * @authors Yang Zhou, Da Li, Ken Yu
 * @brief Decoder for video stored in memory.
 * @copyright CRIPAC 2016
 */
class VideoDecoder {
 private:
  // Variables used by the decoder.
  AVFormatContext *avformat_context_;
  AVCodecContext *codec_context_;
  SwsContext *sws_context_;
  AVIOContextManager *avio_context_manager_;
  AVCodec *codec_;
  AVFrame *frame_raw_, *frame_rgb_;
  uint8_t *frame_;
  AVPacket *packet_;
  int video_ind_;

  inline void clean() {
    if (sws_context_)
      sws_freeContext(sws_context_);
    sws_context_ = NULL;
    if (frame_)
      av_free(frame_);
    frame_ = NULL;
    if (frame_raw_)
      av_frame_free(&frame_raw_);
    frame_raw_ = NULL;
    if (frame_rgb_)
      av_frame_free(&frame_rgb_);
    frame_rgb_ = NULL;
    if (codec_context_)
      avcodec_close(codec_context_);
    codec_context_ = NULL;
    if (avformat_context_) {
      avformat_close_input(&avformat_context_);
      avformat_free_context(avformat_context_);
    }
    avformat_context_ = NULL;

    if (avio_context_manager_)
      delete avio_context_manager_;
    avio_context_manager_ = NULL;

    video_ind_ = 0;
  }
 public:

#define DECODE_SUCCESS                0
#define DECODE_NO_NEXT_FRAME          -1
#define DECODE_FAILURE                -2
#define DECODE_NO_VIDEO_STREAM_FOUND  -3
#define DECODE_NO_CODEC_FOUND         -4

  /**
   * @brief Skip a number of following frames of the video.
   * @return 0: Exactly the number of following frames are skipped successfully;
   *        -1: The number of frames given exceeds the total number of frames. The video reaches the end.
   */
  inline int SkipFrame(int num_frames = 1) {
    for (int i = 0; i < num_frames; ++i) {
      if (av_read_frame(avformat_context_, packet_) < 0)
        return DECODE_NO_NEXT_FRAME;
      av_free_packet(packet_);
    }
    return DECODE_SUCCESS;
  }

  /**
   * @brief Get the next frame_ of the video.
   * @param frame_buf	Buffer for storing the bytes of the frame_.
   * @return 0: Next frame retrieved successfully; -1: No next frame_; -2: Failed to decode.
   */
  inline int NextFrame(unsigned char *frame_buf) {
    // Read a frame to the packet_.
    if (av_read_frame(avformat_context_, packet_) < 0) {
      av_free_packet(packet_);
      return DECODE_NO_NEXT_FRAME;
    }

    if (packet_->stream_index == video_ind_) {
      int got_picture = 1;
      int ret = avcodec_decode_video2(codec_context_, frame_raw_, &got_picture, packet_);
      av_free_packet(packet_);
      if (ret < 0)
        return DECODE_FAILURE;

      /*
       * Seems like some videos have some fake frames preceeding,
       * and when decoding them, the frame_number field of AVCodecContext
       * will not increase.
       * TODO Confirm this.
       */
      if (!codec_context_->frame_number)
        return NextFrame(frame_buf);

      if (got_picture)
        sws_scale(sws_context_, frame_raw_->data, frame_raw_->linesize, 0,
                  codec_context_->height, frame_rgb_->data,
                  frame_rgb_->linesize);

      memcpy(frame_buf, frame_rgb_->data[0], getFrameSize());
      return DECODE_SUCCESS;
    } else {
      av_free_packet(packet_);
      fprintf(stderr, "Failed to decode!\n");
      return DECODE_FAILURE;
    }
  }

  /**
   * @brief Create a decoder for a particular video stored in the memory.
   * @param video_stream	Byte stream of the video.
   * @param buffer_len	Length of the stream.
   */
  inline VideoDecoder() {
    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    avformat_context_ = NULL;
    codec_context_ = NULL;
    sws_context_ = NULL;
    avio_context_manager_ = NULL;
    codec_ = NULL;
    frame_raw_ = NULL;
    frame_rgb_ = NULL;
    frame_ = NULL;
    packet_ = NULL;
  }

  /**
   * @brief Start to decode a video whose bytes are stored in memory.
   * @param video_stream    Byte stream of the video.
   * @param stream_len      Length of the stream.
   * @return    0 on success,
   *            a negative DECODE_NO_VIDEO_STREAM_FOUND on failure to find the video stream,
   *            a negative AVERROR on other failure specified by FFMPEG.
   */
  inline int Decode(const unsigned char *video_stream, size_t stream_len) {
    avio_context_manager_ = new AVIOContextManager(video_stream, stream_len);

    avformat_context_ = avformat_alloc_context();
    avformat_context_->pb = avio_context_manager_->getAVIOContext();

    int ret;
    if ((ret = avformat_open_input(&avformat_context_, NULL, NULL, NULL)) < 0) {
      printf("Couldn't open input stream.\n");
      clean();
      return ret;
    }
    if ((ret = avformat_find_stream_info(avformat_context_, NULL)) < 0) {
      printf("Couldn't find stream information.\n");
      clean();
      return ret;
    }

    // Obtain the video stream of the total set of streams.
    video_ind_ = -1;
    for (int i = 0; i < (int) (avformat_context_->nb_streams); ++i) {
      if (avformat_context_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        video_ind_ = i;
        break;
      }
    }
    if (video_ind_ == -1) {
      fprintf(stderr, "Video stream not found.\n");
      clean();
      return DECODE_NO_VIDEO_STREAM_FOUND;
    }

    codec_context_ = avformat_context_->streams[video_ind_]->codec;
    codec_ = avcodec_find_decoder(codec_context_->codec_id);
    if (codec_ == NULL) {
      fprintf(stderr, "Codec not found.\n");
      clean();
      return DECODE_NO_CODEC_FOUND;
    }
    if ((ret = avcodec_open2(codec_context_, codec_, NULL)) < 0) {
      fprintf(stderr, "Could not open codec.\n");
      clean();
      return ret;
    }
    frame_raw_ = av_frame_alloc();
    frame_rgb_ = av_frame_alloc();

    int num_bytes = avpicture_get_size(AV_PIX_FMT_BGR24, codec_context_->width, codec_context_->height);
    frame_ = (uint8_t *) av_malloc(num_bytes);
    avpicture_fill((AVPicture *) frame_rgb_, frame_, AV_PIX_FMT_BGR24,
                   codec_context_->width, codec_context_->height);
    sws_context_ = sws_getContext(codec_context_->width, codec_context_->height,
                                  codec_context_->pix_fmt, codec_context_->width, codec_context_->height,
                                  AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

    packet_ = (AVPacket *) av_malloc(sizeof(AVPacket));

    return DECODE_SUCCESS;
  }

  inline ~VideoDecoder() {
    clean();
  }

  inline int get_width() const {
    return codec_context_->width;
  }
  inline int get_height() const {
    return codec_context_->height;
  }
  inline int getChannels() const {
    return 3;
  }
  inline int getFrameSize() const {
    return get_width() * get_height() * getChannels();
  }
};

}

#endif // _VIDEO_DECODER_H_
