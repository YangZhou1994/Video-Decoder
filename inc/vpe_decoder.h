/** \file vpe_decoder.h 
 */

#ifndef _VPE_DECODER_H_
#define _VPE_DECODER_H_

#include <vector>

using namespace std;

typedef struct _video_data_t
{
	int width;
	int height;
	int channels;
	vector<char*> frames;
} VideoData;

/** \fn vpeDecoder
 *  \brief video data decoder.
 *  \param[IN] buffer_len - the length of input buffer.
 *  \param[IN] buffer - video stream to decode.
 *  \param[OUT] data - video data after decode (data & head info).
 */
bool vpeDecoder(int buffer_len, unsigned char* buffer, VideoData& data);

#endif // _VPE_DECODER_H_
