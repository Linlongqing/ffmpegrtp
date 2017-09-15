/****************************************************************************
filename:           SendRTP.h
Author:             linshufei
Date:               2017/9/13
Discription:

*****************************************************************************/
#pragma once
#include "jrtp/rtpsession.h"
#include "jrtp/rtpudpv4transmitter.h"
#include "jrtp/rtpipv4address.h"
#include "jrtp/rtpsessionparams.h"
#include "jrtp/rtperrors.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <netinet/in.h>  
#include <arpa/inet.h>  
#endif //_WIN32

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"jrtplib.lib")

class SendRTP{
public:
    jrtplib::RTPSession sess;

    SendRTP(void);
    ~SendRTP(void);
    void CheckError(int rtpErr);
	void SendH264Nalu(uint8_t* h264buf, int bufLen);
	void SendJPEG(uint8_t* JPEGBuf, int bufLen);
    void init(void);
};
