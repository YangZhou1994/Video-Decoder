//TODO get final plath

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

#include "vpe_decoder.h"

#include <jni.h>

using namespace std;

//TODO jobjectArray or jobject & Find org_path
JNIEXPORT jobject JNICALL Java_org_XX(JNIEnv *env, jobject obj, jint len,
		jbyteArray buffer)
{
	jbyte *contentdata = new jbyte[len];
	env->GetByteArrayRegion(buffer, 0, len, contentdata);

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

	int buffer_len = len;
	//Decoding 
	bool ret = vpeDecoder(buffer_len, (unsigned char *) contentdata,
			data);
	if (!ret)
	{
		fprintf(stderr, "Error: decode input video stream FAILED!\n");
		return NULL;
	}
	else
		printf("  ---- Decode input video stream SUCCESSFULLY!\n");

	if (data.frames.empty())
	{
		fprintf(stderr, "Error:There isn't any frames!");
		return NULL;
	}

	jclass framesdata = env->FindClass(
			"org/casia/cripac/isee/vpe/commom/VideoData");
	if (framesdata == NULL)
	{
		fprintf(stderr,
				"Error:Can't find Class:org/casia/cripac/isee/vpe/commom/VideoData!");
		return NULL;
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

	jfieldID heightid = env->GetFieldID(framesdata, "height", "I");
	if (heightid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:height");
		return NULL;
	}
	jfieldID channelsid = env->GetFieldID(framesdata, "channels", "I");
	if (channelsid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:channels");
		return NULL;
	}
	jfieldID framesid = env->GetFieldID(framesdata, "frames", "[[B");
	if (framesid == NULL)
	{
		fprintf(stderr, "Error:Can't find parameter:frames");
		return NULL;
	}

	jobject return_obj = env->NewObject(framesdata, constructor);

	//set width,height,channels
	env->SetIntField(return_obj, widthid, data.width);
	env->SetIntField(return_obj, heightid, data.height);
	env->SetIntField(return_obj, channelsid, data.channels);

	//set byte[][] frames

	//TODO set byte[][]
	jclass objClass = env->FindClass("java/lang/Object");
	jobjectArray temp = env->NewObjectArray((jsize)data.frames.size(), objClass,
			0);
	for (uint i = 0; i < data.frames.size(); ++i)
	{
		jbyteArray getbytes = (env)->NewByteArray(
				(jsize) strlen(data.frames[i]));
		//TODO char* to byte[];
		(env)->SetByteArrayRegion(getbytes, 0,
				(jsize) strlen(data.frames[i]),
				(jbyte*) data.frames[i]);
		env->SetObjectArrayElement(temp, i, getbytes);
		env->DeleteLocalRef(getbytes);
	}

	env->SetObjectField(return_obj, framesid, temp);
	env->DeleteLocalRef(temp);
	//TODO check all the new have been deleted
	delete[] contentdata;

	return return_obj;
}

