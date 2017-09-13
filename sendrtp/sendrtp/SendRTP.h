/**************************************************
Author:             linshufei
Date:               2017/9/13
***************************************************/
#pragma once
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

class SendRTP{
public:
	RTPSession sess;

	SendRTP(void);
	~SendRTP(void);
    void CheckError(int rtperr);
	void SendH264Nalu(uint8_t* m_h264Buf, int buflen);
	void init(void);
};
