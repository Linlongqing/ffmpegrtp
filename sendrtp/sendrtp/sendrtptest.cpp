/*
Here's a small IPv4 example: it asks for a portbase and a destination and
starts sending packets to that destination.
*/
#include "jrtp/rtpsession.h"
#include "jrtp/rtpudpv4transmitter.h"
#include "jrtp/rtpipv4address.h"
#include "jrtp/rtpsessionparams.h"
#include "jrtp/rtperrors.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "H264Encoder.h"

#ifdef _WIN32
#include <winsock2.h>  
#else
#include <netinet/in.h>  
#include <arpa/inet.h>  
#endif //_WIN32

#include <opencv2/opencv.hpp>


#pragma comment(lib,"avcodec.lib")  
#pragma comment(lib,"avdevice.lib")  
#pragma comment(lib,"avfilter.lib")  
#pragma comment(lib,"avformat.lib")  
#pragma comment(lib,"avutil.lib")  
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"jrtplib.lib")

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

using namespace jrtplib;

#define MAXLEN	(RTP_DEFAULTPACKETSIZE - 100)

void checkerror(int rtperr){
    if (rtperr < 0){
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        getchar();
        exit(-1);
    }
}

class MyRTPSession : public RTPSession{
public:
    MyRTPSession(void);
    ~MyRTPSession(void);
    void SendH264Nalu(RTPSession* sess, uint8_t* m_h264Buf, int buflen);
protected:


};

MyRTPSession::MyRTPSession(void){}
MyRTPSession::~MyRTPSession(void){}

void MyRTPSession::SendH264Nalu(RTPSession* sess, uint8_t *h264buf, int buflen){
    int status;
    uint8_t *pSendbuf;

    pSendbuf = h264buf;
    if (buflen <= MAXLEN)
    {
        sess->SetDefaultMark(true);
        status = sess->SendPacket((void *)&pSendbuf[0], buflen);
        checkerror(status);
        printf("send_packt 0 len = %d\n", buflen);

    }
    else if (buflen > MAXLEN)
    {
        int send_packet = 0;
        int all_packet = 0;
        int over_data = 0;
        int iSendLen;

        all_packet = buflen / MAXLEN;
        over_data = buflen % MAXLEN;
        send_packet = 0;
        while ((send_packet<all_packet) || ((send_packet == all_packet) && (over_data > 0)))
        {
            printf("send_packet = %d \n", send_packet);
            /* the first packet or the second last packet */
            if ((0 == send_packet) || (send_packet<all_packet && 0 != send_packet))
            {
                sess->SetDefaultMark(false);
                status = sess->SendPacket((void *)&pSendbuf[send_packet*MAXLEN], MAXLEN);
                checkerror(status);
                //printf("send_packet = %d \n",send_packet);
                send_packet++;
            }
            /* the last packet */
            else if (((all_packet == send_packet) && over_data>0) || ((send_packet == (all_packet - 1)) && over_data == 0))
            {
                sess->SetDefaultMark(true);

                if (over_data > 0)
                {
                    iSendLen = buflen - send_packet*MAXLEN;
                }
                else
                {
                    iSendLen = MAXLEN;
                }

                status = sess->SendPacket((void *)&pSendbuf[send_packet*MAXLEN], iSendLen);
                checkerror(status);
                send_packet++;
                //printf("send_packet = %d \n",send_packet);
            }
        }
    }
}

int main(void)
{

#ifdef WIN32  
    WSADATA dat;
    WSAStartup(MAKEWORD(2, 2), &dat);
#endif // WIN32 

    //opencv
    CvCapture* pCapture = cvCreateCameraCapture(0);
    IplImage* pCVFrame = NULL;
    cv::Mat image;

    int i;
    int num;
    int status;

    RTPSession sess;
    MyRTPSession sender;
    uint16_t portbase = 6666;
    uint16_t destport = 6664;
    uint8_t destip[] = { 127, 0, 0, 1 };

    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;

    // set h264 param
    sessparams.SetUsePredefinedSSRC(true);  //设置使用预先定义的SSRC
    sessparams.SetOwnTimestampUnit(1.0 / 9000.0); // 设置采样间隔
    sessparams.SetAcceptOwnPackets(true);   //接收自己发送的数据包

    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams, &transparams);
    checkerror(status);

    RTPIPv4Address addr(destip, destport);
    status = sess.AddDestination(addr);
    checkerror(status);

    sess.SetDefaultTimestampIncrement(3600);// 设置时间戳增加间隔
    sess.SetDefaultPayloadType(96);
    sess.SetDefaultMark(true);

    int pos = 4;
    int header_flag = 1;
    uint8_t buff[1024 * 100] = { 0 };

    CEncoder encoder(640, 480);  //width=640,height=480;
    while (1)
    {
        pCVFrame = cvQueryFrame(pCapture);
        image = cv::Mat(pCVFrame);
        cv::cvtColor(image, image, CV_BGR2YUV_YV12);
        cv::imshow("image", image);
        char c = cv::waitKey(33);
        if (c == 'q')
        {
            break;  
        }
        encoder.Encode(image.data);
        int dataSize = encoder.packet.size;
        memcpy(&buff[pos], encoder.packet.data, dataSize);
        encoder.FreePacket();

        //buff[pos++] = fgetc(fd);


        if ((buff[pos + 3] == 1) && (buff[pos + 2] == 0) && (buff[pos + 1] == 0) && (buff[pos] == 0))   //4 Byte
        {
            printf("$:%d\n", pos);
            sender.SendH264Nalu(&sess, buff, dataSize);
            buff[0] = 0x00;
            buff[1] = 0x00;
            buff[2] = 0x00;
            buff[3] = 0x01;
            pos = 4;
            RTPTime::Wait(0.03);
        }
        else
        {
            if ((buff[pos + 2] == 1) && (buff[pos + 1] == 0) && (buff[pos + 0] == 0))    //3 Byte
            {
				printf("$$:%d\n", pos);
                sender.SendH264Nalu(&sess, buff, dataSize);
                buff[0] = 0x00;
                buff[1] = 0x00;
                buff[3] = 0x01;
                pos = 3;

                RTPTime::Wait(0.03);
            }
        }
    }
/*    if (pos != 0)
    {
        sender.SendH264Nalu(&sess, buff, pos);
    }*/

#if 0
    for (int i = 0; i < nFrames; i++)
    {
        //读取一帧  
        read_frame_y4m(pPicIn, (hnd_t*)y4m_hnd, i);
        if (i == 0)
            pPicIn->i_pts = i;
        else
            pPicIn->i_pts = i - 1;

        //编码  
        int frame_size = x264_encoder_encode(pX264Handle, &pNals, &iNal, pPicIn, pPicOut);

        if (frame_size >0)
        {

            for (int i = 0; i < iNal; ++i)
            {//将编码数据写入文件t  
                //fwrite(pNals[i].p_payload, 1, pNals[i].i_payload, pFile);  
                //发送编码文件  
                sender.SendH264Nalu(pNals[i].p_payload, pNals[i].i_payload);
                RTPTime::Wait(RTPTime(1, 0));
            }
        }
    }
#endif
    sess.BYEDestroy(RTPTime(10, 0), 0, 0);
    return 0;
}

