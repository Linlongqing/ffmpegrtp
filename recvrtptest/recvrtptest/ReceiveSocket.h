#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

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

    int width;
    int height;

    int Listen();
    int AcceptFromClient();
    int ReceiveFromClient(char* pData, int size);
    int GetSize(int &w, int &h);
};


