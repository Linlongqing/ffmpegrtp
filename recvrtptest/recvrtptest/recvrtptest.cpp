#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "H264Decoder.h"
#include "JPEGDecoder.h"
#include "ReceiveRTP.h"
#include "ReceiveSocket.h"
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#include <winsock2.h>  
#else
#include <netinet/in.h>  
#include <arpa/inet.h>  
#endif //_WIN32

extern "C"{

#ifndef   UINT64_C
#define   UINT64_C(value)__CONCAT(value,ULL)
#endif
}
extern "C"
{
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

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

#if 0
void checkerror(int rtperr)
{
    if (rtperr < 0)
    {
        std::cout << "ERROR: " << RTPGetErrorString(rtperr) << std::endl;
        exit(-1);
    }
}

int main(void)
{

#ifdef WIN32  
    WSADATA dat;
    WSAStartup(MAKEWORD(2, 2), &dat);
#endif // WIN32 

    RTPSession sess;
    uint16_t portbase = 6664;
    int status;
    bool done = false;

    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    sessparams.SetOwnTimestampUnit(1.0 / 9000.0);

    sessparams.SetAcceptOwnPackets(true);

    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams, &transparams);
    checkerror(status);

    sess.BeginDataAccess();
    RTPTime delay(0.001);

    size_t len;
    uint8_t *loaddata;
    RTPPacket *pack;
    uint8_t buff[1024 * 100] = { 0 };
    int pos = 0;

    CDecoder decoder;
    while (!done)
    {
        status = sess.Poll();
        checkerror(status);

        if (sess.GotoFirstSourceWithData())
        {
            do
            {
                while ((pack = sess.GetNextPacket()) != NULL)
                {

                    loaddata = pack->GetPayloadData();
                    len = pack->GetPayloadLength();

                    if (pack->GetPayloadType() == 96) //H264
                    {
                        if (pack->HasMarker()) // the last packet
                        {
                            memcpy(&buff[pos], loaddata, len);
                            decoder.Decode(buff, pos + len, NULL, 1);
                            pos = 0;
                        }
                        else
                        {
                            memcpy(&buff[pos], loaddata, len);
                            pos = pos + len;
                        }
                    }
                    else
                    {
                        printf("!!!  GetPayloadType = %d !!!! \n ", pack->GetPayloadType());
                    }

                    sess.DeletePacket(pack);
                }
            } while (sess.GotoNextSourceWithData());
        }

        RTPTime::Wait(delay);
    }

    sess.EndDataAccess();
    delay = RTPTime(10.0);
    sess.BYEDestroy(delay, 0, 0);

    return 0;
}
#else
int main()
{
    //CH264Decoder decoder;
    CJPEGDecoder decoder;
    //ReceiveRTP receive;
    CReceiveSocket s;
    s.Listen();
    s.AcceptFromClient();
    cv::Mat dstImage(cv::Size(480, 272), CV_8UC1);
    s.ReceiveFromClient((char *)dstImage.data, 130560);
    if (!decoder.Decode(dstImage.data, 130560, NULL))
    {
        int width;
        int height;
        decoder.GetSize(width, height);
        cv::Mat image(cv::Size(width, height), CV_8UC1);
        decoder.GetData(image.data);

        cv::imshow("服务器接收图片", image);
        cv::waitKey(0);
    }

        //Receive a audio/video stream from client
        //receive.Init();

        //while (1)
        //{
        //    if (receive.GetFirstSourceWithData())
        //    {
        //        do
        //        {
        //            int size = receive.GetH264Packet();
        ////int size = receive.GetJPEGPacket();
        //            if (size)
        //            {
        //                if (!decoder.Decode(receive.pBuff, size, NULL))
        //                {
        //		int width;
        //		int height;
        //		decoder.GetSize(width, height);
        //		cv::Mat image(cv::Size(width, height), CV_8UC1);
        //		decoder.GetData(image.data);

        //                    cv::imshow("image", image);
        //                    cv::waitKey(3);
        //		std::cout << "Lena is coming!" << std::endl;
        //                }
        //            }
        //        } while (receive.GotoNextSourceWithData());
        //    }
        //    Sleep(1);
        //}

        //receive.Destroy();

        return 0;
    }

#endif
