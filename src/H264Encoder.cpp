/****************************************************************************
filename:           H264Eecoder.cpp
Author:             linlongqing
Date:               2017/9/12
Discription:

****************************************************************************/
#include "H264Encoder.h"
#include <iostream>

//#define RECODE_STREAM
CRTPEncoder::CRTPEncoder(int width, int height)
{
    count = 0;
    framecnt = 0;

    //注册编码器
    av_register_all();

    //寻找编码器
    pCodec = avcodec_find_encoder(AV_CODEC_ID_H265);
    if (!pCodec)
    {
        printf("Can not find encoder! \n");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);

    //Param that must set

    //最大和最小量化系数，取值范围为0~51。
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 31;

    //编码后的视频帧大小，以像素为单位。
    pCodecCtx->width = width;
    pCodecCtx->height = height;

    //编码后的码率：值越大越清晰，值越小越流畅。
    pCodecCtx->bit_rate = 100000; 

    //每20帧插入一个I帧
    pCodecCtx->gop_size = 20;

    //帧率的基本单位，time_base.num为时间线分子，time_base.den为时间线分母，帧率=分子/分母。
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    //图像色彩空间的格式，采用什么样的色彩空间来表明一个像素点。
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //编码器编码的数据类型
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    //Optional Param  
    //两个非B帧之间允许出现多少个B帧数，设置0表示不使用B帧，没有编码延时。B帧越多，压缩率越高。
    pCodecCtx->max_b_frames = 0;

    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    if (pCodecCtx->codec_id == AV_CODEC_ID_H265){
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }

    //初始化视音频编码器的AVCodecContext
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0)
    {
        printf("Failed to open encoder! \n");
    }

    //Init frame
    pFrame = av_frame_alloc();
    int pictureSize = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    pictureBuf = (uint8_t *)av_malloc(pictureSize);
    avpicture_fill((AVPicture *)pFrame, pictureBuf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;
    pFrame->format = pCodecCtx->pix_fmt;

    //Init packet
    av_new_packet(&packet, pictureSize);
    dataSize = pCodecCtx->width * pCodecCtx->height;
}

CRTPEncoder::~CRTPEncoder()
{
    //销毁
    avcodec_close(pCodecCtx);
    av_free(pFrame);
    av_free(pictureBuf);
    av_free_packet(&packet);
}

int CRTPEncoder::Encode(unsigned char* data)
{
    pFrame->data[0] = data;                         // Y
    pFrame->data[1] = data + dataSize;              // U
    pFrame->data[2] = data + dataSize * 5 / 4;      // V
    pFrame->pts = (count++)*(pCodecCtx->time_base.den) / ((pCodecCtx->time_base.num) * 25);  //时间戳
    memset(&packet, 0, sizeof(AVPacket));

    //Encode
    int gotPicture = 0;
    int ret = avcodec_encode_video2(pCodecCtx, &packet, pFrame, &gotPicture);

    if (ret < 0)
    {
        printf("Failed to encode! \n");
        return -1;
    }

    if (gotPicture == 1)
    {
        printf("Succeed to encode frame: %5d\tsize:%5d\n", framecnt, packet.size);
        framecnt++;

        return 0;
    }

    return -1;
}

