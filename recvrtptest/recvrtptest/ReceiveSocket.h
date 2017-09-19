/****************************************************************************
filename:           ReceiveSocket.h
Author:             linshufei
Date:               2017/9/19
Discription:

*****************************************************************************/

#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
#define MAX_IMAGE_SIZE 1920 * 1080 

typedef struct SImageHeader
{
    int width;
    int height;
    int serverType;
    int dataOffset;
    int dataSize;
} SImageHeader;

class CReceiveSocket
{
public:
    CReceiveSocket();
    ~CReceiveSocket();
    WSADATA wsaData;
    SOCKET sockServer;
    SOCKADDR_IN addrServer;
    SOCKET sockClient;
    SOCKADDR_IN addrClient;

    char* pData;
    int width;
    int height;
    int imageSize;

    int Listen();
    int AcceptFromClient();
    int ReceiveFromClient(char* recvBuf, int recvBufLen);
    int SetSize(int &w, int &h, int &size);
};


