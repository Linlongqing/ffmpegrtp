/****************************************************************************
filename:           SendRTP.h
Author:             linshufei
Date:               2017/9/13
Discription:

*****************************************************************************/
#ifndef ENCODER_H
#define ENCODER_H

#ifdef _WIN32  
//Windows  
extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};
#else  
//Linux...  
#ifdef __cplusplus  
extern "C"
{
#endif  
#include <libavutil/opt.h>  
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#ifdef __cplusplus  
};
#endif  
#endif

class CEncoder
{
private:
    int count;
    int dataSize;
    int framecnt;

    //data
    AVFrame* pFrame = NULL;
    uint8_t* pictureBuf = NULL;

    //encoder message
    AVCodec* pCodec = NULL;
    AVDictionary *param = NULL;
    AVCodecContext* pCodecCtx = NULL;

public:
    AVPacket packet;

    CEncoder(int width, int height);
    ~CEncoder();
    int FreePacket();
    int Encode(unsigned char* data);
    int FlushEncoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
};
#endif //ENCODER_H