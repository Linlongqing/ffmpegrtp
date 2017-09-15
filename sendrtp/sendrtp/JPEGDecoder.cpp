#include "JPEGDecoder.h"



JPEGDecoder::JPEGDecoder()
{
	avcodec_register_all();
	pFormatCtx = avformat_alloc_context();
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	pCodec = avcodec_find_decoder(AV_CODEC_ID_MJPEG);
	if (pCodec == NULL)
	{
		printf("Fail to get decoder !\n");
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);
	if (pCodecCtx == NULL)
	{
		printf("Fail to get decoder context !\n");
	}

	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		printf("Fail to open decoder !\n");
	}

	pFrame = av_frame_alloc();

	frameCount = 0;
}

JPEGDecoder::~JPEGDecoder()
{
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);
	av_frame_free(&pFrame);

	if (packet != NULL)
	{
		delete packet;
	}
}

int JPEGDecoder::Decode(uint8_t *pDataIn, int nInSize, uint8_t *pDataOut)
{
	packet->size = nInSize;
	packet->data = pDataIn;

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

int JPEGDecoder::GetSize(int& width, int& height)
{
	width = pFrame->width;
	height = pFrame->height;

	return 0;
}

int JPEGDecoder::GetData(uint8_t* pData)
{
	memcpy(pData, pFrame->data[0], pFrame->width * pFrame->height * sizeof(uint8_t));
	return 0;
}