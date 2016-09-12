/** \file vpe_decoder.h 
 */

#ifndef _VPE_DECODER_H_
#define _VPE_DECODER_H_

#include <vector>
#include <cstring>
#include <cstdio>

using namespace std;

class VideoData
{
private:
	int width;
	int height;
	int channels;
	vector<unsigned char *> frames;
public:
	inline int getWidth() const { return width; }
	inline int getHeight() const { return height; }
	inline int getChannels() const { return channels; }

	inline const unsigned char* getFrame(int index) const { return frames[index]; }

	inline int getNumFrames() const { return frames.size(); }

	inline int getFrameSize() const
	{
		return width * height * channels * sizeof(char);
	}

	inline void addFrame(const unsigned char *frame)
	{
		int size = getFrameSize();
		unsigned char* copyFrame = new unsigned char[size];
		memcpy(copyFrame, frame, size);
		frames.push_back(copyFrame);
	}

	VideoData(){ frames.clear(); }

	VideoData(int width, int height, int channels) :
			width(width), height(height), channels(channels)
	{
		printf("Creating VideoData with width=%d, height=%d, channels=%d...", width, height, channels);
		frames.clear();
	}

	inline void freeFrame(int index) { if (frames[index]) { delete[] frames[index]; frames[index] = NULL; } }

	~VideoData()
	{
		int numFrames = frames.size();
		for (int i = 0; i < numFrames; ++i)
			if (frames[i])
				delete[] frames[i];
	}
};

/** \fn vpeDecoder
 *  \brief video data decoder.
 *  \param[IN] buffer_len - the length of input buffer.
 *  \param[IN] buffer - video stream to decode.
 *  \param[OUT] data - video data after decode (data & head info).
 */
bool decode(int buffer_len, unsigned char* buffer, VideoData& data);

#endif // _VPE_DECODER_H_
