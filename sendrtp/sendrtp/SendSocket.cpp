#include "SendSocket.h"


CSendSocket::CSendSocket()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //新建客户端socket
    sockClient = socket(AF_INET, SOCK_STREAM, 0);
    //定义要连接的服务端地址
    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(6000);

}


CSendSocket::~CSendSocket()
{
    closesocket(sockClient);
    WSACleanup();

}

int CSendSocket::Connect2Server()
{
    connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));
    return 0;
}

int CSendSocket::Send2Server(char* pData ,int size)
{
    send(sockClient, pData, size, 0);
    return 0;
}

int CSendSocket::GetSize(int w, int h)
{
    width = w;
    height = h;
    return 0;
}