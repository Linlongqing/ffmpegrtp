#include "H264Encoder.h"
#define RECODE_STREAM
CEncoder::CEncoder(int width, int height)
{
    count = 0;
    framecnt = 0;
    av_register_all();

    pFormatCtx = avformat_alloc_context();

    pVideoSt = avformat_new_stream(pFormatCtx, 0);
    if (pVideoSt == NULL)
    {
        printf("Failed to create video stream! \n");
    }

    //Param that must set
    pCodecCtx = pVideoSt->codec;
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

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        printf("Can not find encoder! \n");
    }
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0){
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

#ifdef RECODE_STREAM
    //Open file
    const char* out_file = "recorded.h264";
    av_guess_format(NULL, out_file, NULL);
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0)
    {
        printf("Failed to open output file! \n");
    }

    pFormatCtx->oformat = av_guess_format(NULL, out_file, NULL);

    //Show some Information  
    av_dump_format(pFormatCtx, 0, out_file, 1);
    avformat_write_header(pFormatCtx, NULL);
#endif //RECODE_STREAM
}

CEncoder::~CEncoder()
{
    //Flush Encoder  
    int ret = FlushEncoder(pFormatCtx, 0);
    if (ret < 0) 
    {
        printf("Flushing encoder failed\n");
    }

#ifdef RECODE_STREAM
    av_write_trailer(pFormatCtx);
#endif //RECODE_STREAM

    //Clean  
    if (pVideoSt){
        avcodec_close(pVideoSt->codec);
        av_free(pFrame);
        av_free(pictureBuf);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
}

int CEncoder::Encode(unsigned char* data)
{
    pFrame->data[0] = data;                         //Y
    pFrame->data[1] = data + dataSize;              // U
    pFrame->data[2] = data + dataSize * 5 / 4;      // V
    pFrame->pts = (count++)*(pVideoSt->time_base.den) / ((pVideoSt->time_base.num) * 25);

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
        packet.stream_index = pVideoSt->index;
        ret = av_write_frame(pFormatCtx, &packet);
        return 0;
    }

    return -1;
}

int CEncoder::FreePacket()
{
    av_free_packet(&packet);

    return 0;
}

int CEncoder::FlushEncoder(AVFormatContext *fmt_ctx, unsigned int stream_index)
{
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
        CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
            NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame){
            ret = 0;
            break;
        }
        printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", enc_pkt.size);

        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}