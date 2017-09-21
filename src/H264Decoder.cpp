/****************************************************************************
filename:           H264Decoder.cpp
Author:             linlongqing
Date:               2017/9/12
Discription:

****************************************************************************/
#include "H264Decoder.h"

CH264Decoder::CH264Decoder()
{
    //注册解码器
    avcodec_register_all();

    //分配内存
    pFormatCtx = avformat_alloc_context();
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    //寻找解码器
    pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (pCodec == NULL)
    {
        printf("Fail to get decoder !\n");
    }

    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx == NULL)
    {
        printf("Fail to get decoder context !\n");
    }

    //图像色彩空间的格式，采用什么样的色彩空间来表明一个像素点。
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    //解码器解码的数据类型
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    //初始化视音频编码器的AVCodecContext
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Fail to open decoder !\n");
    }

    pFrame = av_frame_alloc();
    frameCount = 0;
}

CH264Decoder::~CH264Decoder()
{
    //销毁
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
    av_frame_free(&pFrame);

    if (packet != NULL)
    {
        delete packet;
    }
}

int CH264Decoder::Decode(uint8_t *pDataIn, int nInSize)
{
    packet->size = nInSize;
    packet->data = pDataIn;

    //decoder
    int gotPicture;
    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &gotPicture, packet);
    if (ret < 0)
    {
        printf("Decode Error.\n");
    }
    if (gotPicture){
        return 0;
    }

    return -1;
}

int CH264Decoder::GetSize(int& width, int& height)
{
	width = pFrame->width;
	height = pFrame->height;

	return 0;
}

int CH264Decoder::GetData(uint8_t* pData)
{
    int size = pFrame->width * pFrame->height;
	memcpy(pData, pFrame->data[0], size * sizeof(uint8_t));
    memcpy(pData + size, pFrame->data[1], size / 4 * sizeof(uint8_t));
    memcpy(pData + size * 5 / 4, pFrame->data[2], size / 4 * sizeof(uint8_t));
	return 0;
}