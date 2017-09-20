/****************************************************************************
filename:           JPEGEncoder.cpp
Author:             linshufei
Date:               2017/9/15
Discription:

*****************************************************************************/
#pragma once
#include "JPEGEncoder.h"
#include <iostream>

CJPEGEncoder::CJPEGEncoder(int width, int height)
{
    avcodec_register_all();
    pCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (pCodec == NULL)
    {
        printf("Fail to get encoder !\n");
    }

    //Param that must set
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL)
    {
        printf("Fail to get decoder context !\n");
    }

    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ444P;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	//Output some information
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Could not open codec.");
	}

    //init picture
	picture = av_frame_alloc();
	int size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	pictureBuf = (uint8_t *)av_malloc(size);
	if (!pictureBuf)
	{
        std::cout << "av_malloc picture_buf failed!" << std::endl;
	}
	avpicture_fill((AVPicture *)picture, pictureBuf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

    //new packet
    av_new_packet(&packet,size);

}

CJPEGEncoder::~CJPEGEncoder()
{
    av_free(pCodec);
    av_free(pCodecCtx);
    av_free(picture);
    av_free(pictureBuf);
    av_free_packet(&packet);
}

int CJPEGEncoder::Encode(unsigned char* data)
{
	//Write Header
    int ySize = pCodecCtx->width * pCodecCtx->height;
	memcpy(pictureBuf, data, ySize * 3 / 2);
	picture->data[0] = pictureBuf;                    // Y
	picture->data[1] = pictureBuf + ySize;            // U
	picture->data[2] = pictureBuf + ySize * 5 / 4;    // V

	//Encode
    int gotPicture;
	int ret = avcodec_encode_video2(pCodecCtx, &packet, picture, &gotPicture);
    //std::cout << picture->data[0] << std::endl;

	if (ret < 0){
		printf("Encode Error.\n");
		return -1;
	}

	if (gotPicture == 1)
    {
       	printf("Encode Successful.\n");
	}

	return 0;

}

int CJPEGEncoder::SetSize(int width, int height)
{
	inW = width;
	inH = height;
	return 0;
}

