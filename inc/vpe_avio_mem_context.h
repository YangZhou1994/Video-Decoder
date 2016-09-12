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
#ifdef __cplusplus
}
;
#endif

class VpeAvioMemContext
{
public:
	VpeAvioMemContext(char* video_stream, const int stream_len)
	{
		// Output.
		buffer_size = 32768;
		buffer = (char*) av_malloc(buffer_size);

		// Internal buffer.
		pos = 0;
		this->video_stream = video_stream;
		this->video_stream_len = stream_len;
		//printf("1111111111111111\n");
		//ctx = avio_alloc_context((unsigned char*)buffer, buffer_size,
		//      0, this, &VpeAvioMemContext::read, NULL, NULL);
		//printf("2222222222222222\n");
	}

	~VpeAvioMemContext(void)
	{
		// av_free(buffer);
	}

	// Read.
	static int read(void* opaque, unsigned char* buf, int buf_size)
	{
		VpeAvioMemContext* avio_ctx = (VpeAvioMemContext*) opaque;
		// printf("Hey, I am in read()! \n");
		printf("  In callback function: avio_ctx->pos = %d\n", avio_ctx->pos);
		printf("  In callback function: buf_size = %d\n", buf_size);
		// Read from pos to pos + buf_size.
		int ret = -1;
		if (avio_ctx->pos >= avio_ctx->video_stream_len)
		{
			printf("  ==== Callback function DONE! ====\n");
			return -1;
		}
		if (avio_ctx->pos + buf_size > avio_ctx->video_stream_len)
		{
			int len = avio_ctx->video_stream_len - avio_ctx->pos;
			memcpy(buf, avio_ctx->video_stream + avio_ctx->pos, len);
			ret = (int) seek(opaque, buf_size, avio_ctx->pos);
			// printf("==================\n");
			return len;
		}
		else
		{
			memcpy(buf, avio_ctx->video_stream + avio_ctx->pos, buf_size);
			ret = (int) seek(opaque, buf_size, avio_ctx->pos);
			return buf_size;
		}
	}

	// Write.
	static int write(void *opaque, unsigned char *buf, int buf_size)
	{
		return 0;
	}

	// Seek.
	static int64_t seek(void *opaque, int64_t offset, int whence)
	{
		VpeAvioMemContext* avio_ctx = (VpeAvioMemContext*) opaque;
		if (offset + whence > avio_ctx->video_stream_len)
		{
			avio_ctx->pos = avio_ctx->video_stream_len;
			return -1;
		}
		else
		{
			avio_ctx->pos = offset + whence;
			return 0;
		}
	}

public:
	AVIOContext* getAVIO(void)
	{
		return ctx;
	}
	void setAVIO(void)
	{
		// printf("1111111111111111\n");
		ctx = avio_alloc_context((unsigned char*) buffer, buffer_size, 0, this,
				&VpeAvioMemContext::read, NULL, NULL);
		// printf("2222222222222222\n");
	}
	//void destroy(void) {
	//    av_free(buffer);
	//}

private:
	// Output
	int buffer_size;
	char* buffer;

	// Internal
	int pos;
	char* video_stream;
	int video_stream_len;

	AVIOContext* ctx = NULL;

};
// VpeAvioMemContext

#endif  // _VPE_AVIO_MEM_CONTEXT_H_
