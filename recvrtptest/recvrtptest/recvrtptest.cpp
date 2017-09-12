/*=============================================================================
*     FileName: receive.cpp
*         Desc: reveive h.264 from RTP server
*       Author: licaibiao
*   LastChange: 2017-04-22
* =============================================================================*/
#include "jrtp/rtpsession.h"
#include "jrtp/rtpudpv4transmitter.h"
#include "jrtp/rtpipv4address.h"
#include "jrtp/rtpsessionparams.h"
#include "jrtp/rtperrors.h"
#include "jrtp/rtppacket.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "H264Decoder.h"

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
    sessparams.SetOwnTimestampUnit(1.0 / 10.0);

    sessparams.SetAcceptOwnPackets(true);

    transparams.SetPortbase(portbase);
    status = sess.Create(sessparams, &transparams);
    checkerror(status);

    sess.BeginDataAccess();
    RTPTime delay(0.001);
    RTPTime starttime = RTPTime::CurrentTime();

    FILE *fd;
    size_t len;
    uint8_t *loaddata;
    RTPPacket *pack;
    uint8_t buff[1024 * 100] = { 0 };
    int pos = 0;

    fd = fopen("./test_recv.h264", "wb+");
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
                            fwrite(buff, 1, pos + len, fd);
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
        RTPTime t = RTPTime::CurrentTime();
        t -= starttime;
        //if (t > RTPTime(40.0))
        //done = true;
    }
    fclose(fd);

    sess.EndDataAccess();
    delay = RTPTime(10.0);
    sess.BYEDestroy(delay, 0, 0);

    return 0;
}