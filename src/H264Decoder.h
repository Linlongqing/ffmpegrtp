#ifndef __DECODER_H
#define __DECODER_H
#include <stdio.h>
#include <opencv2/opencv.hpp>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL/SDL.h>
#ifdef __cplusplus
};
#endif
#endif

class CDecoder
{
private:
    //FFmpeg
    int             frameCount;
    int             videoindex;
    AVCodec         *pCodec;
    AVFrame         *pFrame;
    AVPacket        *packet;
    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx;
public:
    CDecoder();
    ~CDecoder();
    int Decode(uint8_t *pDataIn, int nInSize, uint8_t *pDataOut, int nOutSize);
};

#endif