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
}
;
#endif

#include <algorithm>
#include <cassert>

namespace videodecoder
{
/**
 * The AVIOContextManager class manages an I/O context for FFMPEG (AVIOContext).
 * Each context corresponds to a video.
 */
class AVIOContextManager
{
public:
	/**
	 * Create a AVIO context manager with a video stream stored in memory.
	 * @param video_stream		The video stream stored in memory.
	 * @param stream_len		Length of the video stream.
	 * @param buf_size			Size of buffer to be allocated for AVIOContext.
	 * @throw std::bad_alloc	On failure allocating memory.
	 */
	inline AVIOContextManager(const unsigned char* video_stream, const int64_t stream_len, size_t buf_size = 32768) :
			pos(0),
			video_stream(video_stream),
			stream_len(stream_len)
	{
		unsigned char* buffer = (unsigned char*) av_malloc(buf_size);
		if (NULL == buffer)
			throw std::bad_alloc();

		ctx = avio_alloc_context(buffer, buf_size, 0, this, &AVIOContextManager::read_packet, NULL, seek);
		if (NULL == ctx)
		{
			av_free(buffer);
			throw std::bad_alloc();
		}

		// We don't need to free the buffer. Instead, we free the buffer in the context.
	}

	/**
	 * Free the AVIOContext as well as buffer allocated for it.
	 */
	inline ~AVIOContextManager(void)
	{
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
	static int read_packet(void* opaque, unsigned char* buf, int buf_size)
	{
		// Recover the AVIO context.
		AVIOContextManager* avio_ctx = (AVIOContextManager*) opaque;

		// If the position already exceeds the video's length.
		if (avio_ctx->pos >= avio_ctx->stream_len)
			return -1;
		else
		{
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
	static int64_t seek(void *opaque, int64_t offset, int whence)
	{
		// Recover the AVIO context.
		AVIOContextManager* avio_ctx = (AVIOContextManager*) opaque;

		// If the destination exceeds the video length.
		if (offset + whence > avio_ctx->stream_len)
		{
			// Set the current position to the end of the video.
			avio_ctx->pos = avio_ctx->stream_len;
			// Return failure sign.
			return -1;
		}
		else
		{
			// Modify the current position to the destination.
			avio_ctx->pos = offset + whence;
			// Return success sign.
			return 0;
		}
	}

	/**
	 * Get the AVIOContext the manager holds.
	 */
	AVIOContext* getAVIOContext(void)
	{
		return ctx;
	}
private:
	// Internal
	int64_t pos;
	const unsigned char* video_stream;
	int64_t stream_len;

	AVIOContext* ctx = NULL;
};
}

#endif  // _VPE_AVIO_MEM_CONTEXT_H_
