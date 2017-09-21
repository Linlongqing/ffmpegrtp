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
    int width;          //图片的宽度
    int height;         //图片的高度
    int serverType;     //请求的服务类型
    int dataOffset;     //图片数据头的位置
    int dataSize;       //图片大小
} SImageHeader;

class CReceiveSocket
{
public:
    CReceiveSocket();
    ~CReceiveSocket();
    WSADATA wsaData;
    SOCKET sockServer;          //服务器Socket
    SOCKADDR_IN addrServer;     //服务器地址
    SOCKET sockClient;          //客户端Socket
    SOCKADDR_IN addrClient;     //客户端地址

    char* pData;
    int width;
    int height;
    int imageSize;

    int Listen();
    int AcceptFromClient();
    int ReceiveFromClient(char* recvBuf, int recvBufLen);
    int SetSize(int &w, int &h, int &size);
};


