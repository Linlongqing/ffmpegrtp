/****************************************************************************
filename:           ReceiveSocket.cpp
Author:             linshufei
Date:               2017/9/19
Discription:

*****************************************************************************/

#include "ReceiveSocket.h"

CReceiveSocket::CReceiveSocket()
{
    pData = new char[MAX_IMAGE_SIZE];
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //INADDR_ANY表示任何IP
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(6000); //绑定端口6000
    bind(sockServer, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));
}

CReceiveSocket::~CReceiveSocket()
{
    if (pData == NULL)
    {
        delete[] pData;
        pData = NULL;
    }

    closesocket(sockClient);
    WSACleanup();
}

//监听
int CReceiveSocket::Listen()
{
    listen(sockServer, 1); //1为等待连接数目
    std::cout << "服务器已启动：" << std::endl << "监听中" << std::endl;
    return 0;
}

//会阻塞进程，直到有客户端连接上来为止
int CReceiveSocket::AcceptFromClient()
{
    int len = sizeof(SOCKADDR);
    sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
    return 0;
}

//接收客户端数据
int CReceiveSocket::ReceiveFromClient(char* recvBuf, int recvBufLen)
{
    int pos = 0;
    SImageHeader header;

    int len = recv(sockClient, (char *)&header, sizeof(SImageHeader), 0);
    while (1)
    {
        len = recv(sockClient, &recvBuf[pos], 1024, 0);
        pos += len;
        std::cout << "pos:" << pos << "/"<< header.dataSize <<std::endl;
        if (pos >= header.dataSize)
        {
            break;
        }
    }

    std::cout << "length:" << len << std::endl;
    memcpy(pData, recvBuf, header.dataSize);
    SetSize(header.width, header.height, header.dataSize);
    return 0;
}

int CReceiveSocket::SetSize(int &w, int &h ,int &size)
{
    width = w;
    height = h;
    imageSize = size;
    return 0;
}