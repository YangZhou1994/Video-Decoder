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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <jni.h>

#include <video_decoder.hpp>
#include <org_casia_cripac_isee_vpe_util_VideoDecoder.h>

using namespace std;
using namespace videodecoder;

JNIEXPORT jlong JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_initialize(
		JNIEnv *env, jobject obj, jbyteArray buffer)
{
	jsize buffer_len = (env)->GetArrayLength(buffer);
	jbyte *contentdata = new jbyte[buffer_len];
	env->GetByteArrayRegion(buffer, 0, buffer_len, contentdata);
	VideoDecoder* p = new VideoDecoder((const unsigned char *) contentdata,
			buffer_len);
	return (jlong) p;
}

JNIEXPORT jbyteArray JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_nextFrame(
		JNIEnv *env, jobject obj, jlong raw_pointer)
{
	VideoDecoder *p = (VideoDecoder *) raw_pointer;
	unsigned char *frame_buf = new unsigned char[p->getFrameSize()];
	int ret = p->NextFrame(frame_buf);
	if (ret)
		return NULL;

	jbyteArray getbytes = (env)->NewByteArray(p->getFrameSize());
	env->SetByteArrayRegion(getbytes, 0, p->getFrameSize(),
			(const jbyte*) frame_buf);
	delete[] frame_buf;
	return getbytes;
}

JNIEXPORT int JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_skipFrame
  (JNIEnv *env, jobject obj, jlong pointer, jint num_frames)
{
	return ((VideoDecoder *) pointer)->SkipFrame(num_frames);
}

JNIEXPORT void JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_free(JNIEnv *env,
		jobject obj, jlong pointer)
{
	delete (VideoDecoder *) pointer;
}

JNIEXPORT jint JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_getWidth(
		JNIEnv *env, jobject obj, jlong pointer)
{
	return ((VideoDecoder *) pointer)->get_width();
}

JNIEXPORT jint JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_getHeight(
		JNIEnv *env, jobject obj, jlong pointer)
{
	return ((VideoDecoder *) pointer)->get_height();
}

JNIEXPORT jint JNICALL Java_org_casia_cripac_isee_vpe_util_VideoDecoder_getChannels(
		JNIEnv *env, jobject obj, jlong pointer)
{
	return ((VideoDecoder *) pointer)->getChannels();
}

