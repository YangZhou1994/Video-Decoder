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

/**
 * \file  vpe_avio_mem_context.h
 * \date  2016-08-09
 * \email da.li@cripac.ia.ac.cn
 */

#ifndef _VPE_AVIO_MEM_CONTEXT_H_
#define _VPE_AVIO_MEM_CONTEXT_H_

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#ifdef __cplusplus
extern "C"
{
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/mem.h"
#ifdef __cplusplus
};
#endif

#include <algorithm>
#include <cassert>

/**
 * Adaption to lower version of AVCodec.
 */
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free av_free
#define AV_PIX_FMT_BGR24 PIX_FMT_BGR24
#endif

namespace isee {
/**
 * The AVIOContextManager class manages an I/O context for FFMPEG (AVIOContext).
 * Each context corresponds to a video.
 */
class AVIOContextManager {
 public:
  /**
   * Create a AVIO context manager with a video stream stored in memory.
   * @param video_stream		The video stream stored in memory.
   * @param stream_len		Length of the video stream.
   * @param buf_size			Size of buffer to be allocated for AVIOContext.
   * @throw std::bad_alloc	On failure allocating memory.
   */
  inline AVIOContextManager(const unsigned char *video_stream, const int64_t stream_len, size_t buf_size = 32768) :
      pos(0),
      video_stream(video_stream),
      stream_len(stream_len),
      ctx(NULL) {
    unsigned char *buffer = (unsigned char *) av_malloc(buf_size);
    if (NULL == buffer)
      throw std::bad_alloc();

    // Here we use our own seek function, but seems like setting it as NULL also works.
    // TODO: If any problem occurs, please check here.
    ctx = avio_alloc_context(buffer, buf_size, 0, this, &AVIOContextManager::read_packet, NULL, seek);
    if (NULL == ctx) {
      av_free(buffer);
      throw std::bad_alloc();
    }

    // We don't need to free the buffer. Instead, we free the buffer in the context.
  }

  /**
   * Free the AVIOContext as well as buffer allocated for it.
   */
  inline ~AVIOContextManager(void) {
    av_free(ctx->buffer);
    av_free(ctx);
  }

  /**
   * A function for refilling the buffer.
   * @param	opaque		Pointer to an AvioMemContext object.
   * @param	buf			Buffer for decoding.
   * @param	buf_size	Size of the buffer.
   * @return 	Size of data filled or -1 on failure.
   */
  static int read_packet(void *opaque, unsigned char *buf, int buf_size) {
    // Recover the AVIO context.
    AVIOContextManager *avio_ctx = (AVIOContextManager *) opaque;

    // If the position already exceeds the video's length.
    if (avio_ctx->pos >= avio_ctx->stream_len)
      return -1;
    else {
      // Count the size of data to fill into the buffer.
      // Should not exceed neither the size of buffer nor the size of rest data.
      int64_t size_to_fill = std::min(int64_t(buf_size),
                                      avio_ctx->stream_len - avio_ctx->pos);
      // Copy the data to the buffer.
      memcpy(buf, avio_ctx->video_stream + avio_ctx->pos, size_to_fill);
      // Modify current location.
      int ret = seek(opaque, size_to_fill, avio_ctx->pos);
      assert(!ret);
      // Return the size of resting data.
      return size_to_fill;
    }
  }

  /**
   * A function for seeking to specified byte position.
   * @param	opaque	Pointer to an AvioMemContext object.
   * @param	offset	Offset from the whence to seek.
   * @param	whence	Whence from which to seek.
   * @return 	0 on success or -1 on failure.
   */
  static int64_t seek(void *opaque, int64_t offset, int whence) {
    // Recover the AVIO context.
    AVIOContextManager *avio_ctx = (AVIOContextManager *) opaque;

    // Sometimes the whence is different from the position recorded by us.
    // We don't know why, but omitting requests in these situations solve problem.
    // TODO: Figure out why do avcodec input these strange whences.
    if (whence != avio_ctx->pos)
      return -1;

    // If the destination exceeds the video length.
    if (offset + whence > avio_ctx->stream_len) {
      // Set the current position to the end of the video.
      avio_ctx->pos = avio_ctx->stream_len;
      // Return failure sign.
      return -1;
    } else {
      // Modify the current position to the destination.
      avio_ctx->pos = offset + whence;
      // Return success sign.
      return 0;
    }
  }

  /**
   * Get the AVIOContext the manager holds.
   */
  AVIOContext *getAVIOContext(void) {
    return ctx;
  }
 private:
  // Internal
  int64_t pos;
  const unsigned char *video_stream;
  int64_t stream_len;

  AVIOContext *ctx;
};
}

#endif  // _VPE_AVIO_MEM_CONTEXT_H_
