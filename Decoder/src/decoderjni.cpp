//TODO get final plath

#include <decoder.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include "com_vpe_transfer_Download.h"
#include <jni.h>

using namespace std;

//TODO jobjectArray or jobject & Find org_path
//Change it
JNIEXPORT jobject JNICALL Java_com_vpe_transfer_Download_decode(JNIEnv *env,
		jclass obj, jint buffer_len, jbyteArray buffer)
{
	jbyte *contentdata = new jbyte[buffer_len];
	env->GetByteArrayRegion(buffer, 0, buffer_len, contentdata);

	VideoData data;
	//VideoData have been defined in vpe_decoder.h as a struct
	/*
	 typedef struct _video_data_t{
	 int width;
	 int height;
	 int channels;
	 vector<char*> frames;
	 }Videodata
	 */

	//Decoding 
	bool ret = decode(buffer_len, (unsigned char *) contentdata, data);
	if (!ret)
	{
		fprintf(stderr, "Error: decode input video stream FAILED!\n");
		return NULL;
	}
	else
		printf("  ---- Decode input video stream SUCCESSFULLY!\n");

	if (!data.getNumFrames())
	{
		fprintf(stderr, "Error:There isn't any frames!");
		return NULL;
	}
	else
	{
		printf("  ---- Begin:Transfer to JAVA data.\n");
	}
	//change it
	jclass framesdata = env->FindClass("com/vpe/transfer/VideoData");
	if (framesdata == NULL)
	{
		fprintf(stderr, "Error:Can't find Class:com/vpe/transfer/VideoData");
		return NULL;
	}
	else
	{
		printf("Get Class: VideoData\n");
	}
	//TODO find what's mean of the jmethodID&&GetMethodID
	//Q: (Is this step  necessary?)
	/*
	 the class is like follow:
	 public class VideoData{
	 public int width;
	 public int height;
	 public int channels;
	 public byte[][] frames;
	 }
	 */
	jmethodID constructor = env->GetMethodID(framesdata, "<init>", "()V");

	jfieldID widthid = env->GetFieldID(framesdata, "width", "I");
	if (widthid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:width");
		return NULL;
	}
	else
	{
		printf("Get widthid!\n");
	}
	jfieldID heightid = env->GetFieldID(framesdata, "height", "I");
	if (heightid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:height");
		return NULL;
	}
	else
	{
		printf("Get heightid!\n");
	}
	jfieldID channelsid = env->GetFieldID(framesdata, "channels", "I");
	if (channelsid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:channels");
		return NULL;
	}
	else
	{
		printf("Get channelsid!\n");
	}
	jfieldID framesid = env->GetFieldID(framesdata, "frames", "[[B");
	if (framesid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:frames");
		return NULL;
	}
	else
	{
		printf("Get framesid!\n");
	}
	jobject return_obj = env->NewObject(framesdata, constructor);
	//set width,height,channels
	env->SetIntField(return_obj, widthid, data.getWidth());
	env->SetIntField(return_obj, heightid, data.getHeight());
	env->SetIntField(return_obj, channelsid, data.getChannels());

	//set byte[][] frames

	//TODO set byte[][]
	jclass objClass = env->FindClass("java/lang/Object");
	jobjectArray temp = env->NewObjectArray((jsize) data.getNumFrames(),
			objClass, 0);
	for (uint i = 0; i < data.getNumFrames(); ++i)
	{
		printf("Before creating new byte array: %d\n", i);
		jbyteArray getbytes = (env)->NewByteArray((jsize) data.getFrameSize());
		printf("After creating new byte array: %d\n", i);
		(env)->SetByteArrayRegion(getbytes, 0, data.getFrameSize(),
				(jbyte*) data.getFrame(i));
		data.freeFrame(i);
		env->SetObjectArrayElement(temp, i, getbytes);
		env->DeleteLocalRef(getbytes);
	}

	env->SetObjectField(return_obj, framesid, temp);
	env->DeleteLocalRef(temp);
	//TODO check all the new have been deleted
	delete[] contentdata;

	return return_obj;
}

