/****************************************************************************
filename:           SendSocket.h
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
#define DATA_ADDR 5;
#define SERVER_TYPE 0;

typedef struct SImageHeader
{
    int width;
    int height;
    int serverType;
    int dataOffset;
    int dataSize;
} SImageHeader;

class CSendSocket
{
public:
    WSADATA wsaData;
    SOCKET sockClient;  //客户端Socket
    SOCKADDR_IN addrServer;  //服务端地址

    int width;
    int height;

    CSendSocket();
    ~CSendSocket();
    int Connect2Server();
    int Send2Server(char* pData, int size);
    int GetSize(int w, int h);
};

