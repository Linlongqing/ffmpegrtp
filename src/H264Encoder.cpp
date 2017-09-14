/****************************************************************************
filename:           H264Eecoder.cpp
Author:             linlongqing
Date:               2017/9/12
Discription:

****************************************************************************/
#include "H264Encoder.h"
//#define RECODE_STREAM
CEncoder::CEncoder(int width, int height)
{
    count = 0;
    framecnt = 0;
    av_register_all();

    pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!pCodec)
    {
        printf("Can not find encoder! \n");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);

    //Param that must set
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->gop_size = 20;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;

    pCodecCtx->codec_id = AV_CODEC_ID_H264;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    //Optional Param  
    pCodecCtx->max_b_frames = 3;

    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    if (pCodecCtx->codec_id == AV_CODEC_ID_H265){
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }


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

    av_new_packet(&packet, pictureSize);
    dataSize = pCodecCtx->width * pCodecCtx->height;
}

CEncoder::~CEncoder()
{
    avcodec_close(pCodecCtx);
    av_free(pFrame);
    av_free(pictureBuf);
}

int CEncoder::Encode(unsigned char* data)
{
    pFrame->data[0] = data;                         // Y
    pFrame->data[1] = data + dataSize;              // U
    pFrame->data[2] = data + dataSize * 5 / 4;      // V
    pFrame->pts = (count++)*(pCodecCtx->time_base.den) / ((pCodecCtx->time_base.num) * 25);

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

int CEncoder::FreePacket()
{
    av_free_packet(&packet);
    return 0;
}
