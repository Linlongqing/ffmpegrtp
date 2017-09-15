/****************************************************************************
filename:           JPEGEncoder.cpp
Author:             linshufei
Date:               2017/9/15
Discription:

*****************************************************************************/
#pragma once
#include "JPEGEncoder.h"
#include <iostream>

int CJPEGEncoder::Encoder(unsigned char* data)
{
	AVOutputFormat* fmt;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;

	uint8_t* picture_buf;
	AVFrame* picture;
	int y_size;
	int got_picture = 0;
	int size;

	int ret = 0;

    avcodec_register_all();

    pCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (pCodec == NULL)
    {
        printf("Fail to get decoder !\n");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL)
    {
        printf("Fail to get decoder context !\n");
    }

    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ444P;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

	pCodecCtx->width = inW;
	pCodecCtx->height = inH;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	//Output some information
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
		printf("Could not open codec.");
		return -1;
	}

	picture = av_frame_alloc();
	size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *)av_malloc(size);
	if (!picture_buf)
	{
		return -1;
	}
	avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

	//Write Header
	y_size = pCodecCtx->width * pCodecCtx->height;
	memcpy(picture_buf, data, y_size * 3 / 2);
	picture->data[0] = picture_buf;                     // Y
	picture->data[1] = picture_buf + y_size;            // U
	picture->data[2] = picture_buf + y_size * 5 / 4;    // V

	//Encode
	ret = avcodec_encode_video2(pCodecCtx, &packet, picture, &got_picture);

	if (ret < 0){
		printf("Encode Error.\n");
		return -1;
	}
	if (got_picture == 1){
	}

	//av_free_packet(&packet);

	//Write Trailer

	printf("Encode Successful.\n");


    av_free(picture);
    av_free(picture_buf);

	return 0;

}

int CJPEGEncoder::SetSize(int width, int height)
{
	inW = width;
	inH = height;
	return 0;
}

int CJPEGEncoder::FreePacket()
{
	av_free_packet(&packet);
	return 0;
}