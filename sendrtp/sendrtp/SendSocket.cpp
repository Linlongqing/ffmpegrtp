/****************************************************************************
filename:           SendSocket.cpp
Author:             linshufei
Date:               2017/9/19
Discription:

*****************************************************************************/

#include "SendSocket.h"
#include "dataheader.h"

CSendSocket::CSendSocket()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //新建客户端socket
    sockClient = socket(AF_INET, SOCK_STREAM, 0);

    //定义要连接的服务端地址
    addrServer.sin_addr.S_un.S_addr = inet_addr("10.10.10.163"); //118.31.16.221  //10.10.10.163  //192.168.179.130
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(12007);
}

CSendSocket::~CSendSocket()
{
    //closesocket(sockClient);
    //WSACleanup();
}

//请求连接服务器  
int CSendSocket::Connect2Server()
{
    int ret = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));
    if (ret < 0)
    {
        std::cout << "connect failed!" << std::endl;
        return -1;
    }
    return 0;
}

//将数据发送到服务器端，输入为指向数据的指针pData和数据的大小size
int CSendSocket::Send2Server(char* pData ,int size)
{
    char* pictureBuf;
    pictureBuf = new char[MAX_IMAGE_SIZE];
    char* headerBuf = new char[sizeof(SImageHeader)];

    //写数据头
    SImageHeader imageHeader;
    imageHeader.serverType = 0;
    imageHeader.headerType = KRJPEG;
    imageHeader.width = width;
    imageHeader.height = height;
    imageHeader.dataSize = size;
    imageHeader.dataOffset = sizeof(imageHeader);
    imageHeader.headerCheckCode = width + height;
    
    memcpy(headerBuf, &imageHeader, sizeof(imageHeader));
    //send header
    send(sockClient, headerBuf, sizeof(imageHeader), 0);

    //send picture
    uint32_t tailerCheckCode = size;
    memcpy(pictureBuf, pData, size);
    memcpy(&pictureBuf[size], &tailerCheckCode, sizeof(uint32_t));
    send(sockClient, pictureBuf, size + sizeof(uint32_t), 0);
    std::cout << "send picture size: " << size + sizeof(imageHeader) + sizeof(uint32_t) << std::endl;

    delete headerBuf;
    delete pictureBuf;

    return 0;
}

int CSendSocket::GetSize(int w, int h)
{
    width = w;
    height = h;
    return 0;
}
typedef struct obj
{
    uint32_t type;
    uint32_t confidence;
};

int CSendSocket::RecvRes()
{
    //receive the response from server
    //listen(sockClient, 1); //listen and wait for the server to link
    //std::cout << "Client start waitting for response" << std::endl;

    //int len = sizeof(SOCKADDR);
    //sockServer = accept(sockClient, (SOCKADDR*)& addrServer, &len);

    obj Res[5];
    int response = recv(sockClient, (char *)Res, 5 * sizeof(obj), 0);
    for (auto i : Res)
    {
        std::cout << "class: "<< i.type << " - p: " << i.confidence / 65535.0 << std::endl;
    }
    return 0;
}

int CSendSocket::RecvImage(char* &pImage, int &sizeImage)
{
    char* recvBuf = new char[MAX_IMAGE_SIZE];
    int recvBufLen = MAX_IMAGE_SIZE;
    int pos = 0;
    SImageHeader header;
    int len = recv(sockClient, (char*)&header, sizeof(SImageHeader), 0);
    while (true)
    {
        len = recv(sockClient, &recvBuf[pos], 1024, 0);
        pos += len;
        std::cout << "pos:" << pos << "/" << header.dataSize << std::endl;
        if (pos >= header.dataSize)
        {
            //send(sockClient, "linglongqing", 20, 1);
            break;
        }
    }

    std::cout << "length:" << len << std::endl;
    memcpy(pImage, recvBuf, header.dataSize);
    sizeImage = header.dataSize;

    return 0;
}