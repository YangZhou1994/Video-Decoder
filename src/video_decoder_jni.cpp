#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include <jni.h>

#include <org_cripac_isee_vpe_util_Decoder.h>
#include <video_decoder.hpp>

using namespace std;
//using namespace videodecoder;
//
//JNIEXPORT jlong JNICALL Java_org_cripac_isee_vpe_util_Decoder_initialize(
//		JNIEnv *env, jobject obj, jbyteArray buffer)
//{
//	jsize buffer_len = (env)->GetArrayLength(buffer);
//	jbyte *contentdata = new jbyte[buffer_len];
//	env->GetByteArrayRegion(buffer, 0, buffer_len, contentdata);
//	VideoDecoder* p = new VideoDecoder((const unsigned char *) contentdata,
//			buffer_len);
//	return (jlong) p;
//}
//
//JNIEXPORT jbyteArray JNICALL Java_org_cripac_isee_vpe_util_Decoder_nextFrame(
//		JNIEnv *env, jobject obj, jlong pointer)
//{
//	VideoDecoder *p = (VideoDecoder *) pointer;
//	unsigned char *frame_buf = new unsigned char[p->getFrameSize()];
//	int ret = p->NextFrame(frame_buf);
//	if (!ret)
//		return NULL;
//
//	jbyteArray getbytes = (env)->NewByteArray(p->getFrameSize());
//	env->SetByteArrayRegion(getbytes, 0, p->getFrameSize(),
//			(const jbyte*) frame_buf);
//	delete[] frame_buf;
//	return getbytes;
//}
//
//JNIEXPORT void JNICALL Java_org_cripac_isee_vpe_util_Decoder_free(JNIEnv *env,
//		jobject obj, jlong pointer)
//{
//	delete (VideoDecoder *) pointer;
//}
//
//JNIEXPORT jint JNICALL Java_org_cripac_isee_vpe_util_Decoder_getWidth(
//		JNIEnv *env, jobject obj, jlong pointer)
//{
//	VideoDecoder *p = (VideoDecoder *) pointer;
//	return p->get_width();
//}
//
//JNIEXPORT jint JNICALL Java_org_cripac_isee_vpe_util_Decoder_getHeight(
//		JNIEnv *env, jobject obj, jlong pointer)
//{
//	VideoDecoder *p = (VideoDecoder *) pointer;
//	return p->get_height();
//}
//
//JNIEXPORT jint JNICALL Java_org_cripac_isee_vpe_util_Decoder_getChannels(
//		JNIEnv *env, jobject obj, jlong pointer)
//{
//	VideoDecoder *p = (VideoDecoder *) pointer;
//	return p->getChannels();
//}

