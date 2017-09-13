#include "H264Decoder.h"
//#define FROM_FILE
CDecoder::CDecoder()
{
    avcodec_register_all();
    pFormatCtx = avformat_alloc_context();
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

#ifdef FROM_FILE
    char filepath[] = "test_recv.h264";
    pFormatCtx = avformat_alloc_context();
    if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0){
        printf("Couldn't open input stream.\n");
        getchar();
    }
    if (avformat_find_stream_info(pFormatCtx, NULL)<0){
        printf("Couldn't find stream information.\n");
    }

    videoindex = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
        videoindex = i;
        break;
    }
    if (videoindex == -1){
        printf("Didn't find a video stream.\n");
    }
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
#else
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

    //av_init_packet(packet);
#endif //FROM_FILE

    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Fail to open decoder !\n");
    }

    pFrame = av_frame_alloc();

    frameCount = 0;
}

CDecoder::~CDecoder()
{
    avcodec_close(pCodecCtx);
    av_free(pCodecCtx);
    av_frame_free(&pFrame);

    if (packet != NULL)
    {
        delete packet;
    }
}

int CDecoder::Decode(uint8_t *pDataIn, int nInSize, uint8_t *pDataOut, int nOutSize)
{

#ifdef FROM_FILE
    int ret = av_read_frame(pFormatCtx, packet);
#else
    int ret = 0;
    packet->size = nInSize;
    packet->data = pDataIn;
#endif

    int gotPicture;
    int ret0 = avcodec_decode_video2(pCodecCtx, pFrame, &gotPicture, packet);
    if (ret0 < 0)
    {
        printf("Decode Error.\n");
    }
    if (gotPicture){
        cv::Mat img(pFrame->height, pFrame->width, CV_8UC1);
        memcpy(img.data, pFrame->data[0], img.cols * img.rows * sizeof(char));
        cv::imshow("image", img);
        cv::waitKey(33);
    }

    return ret;
}