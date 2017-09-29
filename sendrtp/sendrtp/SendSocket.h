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
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif
#define MAX_IMAGE_SIZE 1920 * 1080 
#define DATA_ADDR 5;
#define SERVER_TYPE 0;

class CSendSocket
{
public:
    WSADATA wsaData;
    SOCKET sockClient;          //客户端Socket
    SOCKET sockServer;          //服务器Socket
    SOCKADDR_IN addrClient;     //客户端地址
    SOCKADDR_IN addrServer;     //服务端地址

    int width;
    int height;

    CSendSocket();
    ~CSendSocket();
    int Connect2Server();  
    int Send2Server(char* pData, int size);
    int GetSize(int w, int h);
    int RecvRes();
    int RecvImage(char* &pImage, int &sizeImage);
};

