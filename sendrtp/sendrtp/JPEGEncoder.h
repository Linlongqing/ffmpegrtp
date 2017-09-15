/****************************************************************************
filename:           JPEGEncoder.h
Author:             linshufei
Date:               2017/9/15
Discription:

*****************************************************************************/
#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef __cplusplus
};
#endif
#endif

class CJPEGEncoder
{
public:
	AVPacket packet;

	CJPEGEncoder(){};
	~CJPEGEncoder(){};
	int Encoder(unsigned char* data);
	int GetSize(int width, int height);
	int FreePacket();
private:
	int inW;
	int inH;
};