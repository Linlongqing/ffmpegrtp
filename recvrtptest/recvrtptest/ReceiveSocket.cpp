#include "ReceiveSocket.h"


CReceiveSocket::CReceiveSocket()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY); //INADDR_ANY表示任何IP
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(6000); //绑定端口6000
    bind(sockServer, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

}


CReceiveSocket::~CReceiveSocket()
{
    closesocket(sockClient);
    WSACleanup();

}

int CReceiveSocket::Listen()
{
    listen(sockServer, 1); //1为等待连接数目
    std::cout << "服务器已启动：" << std::endl << "监听中" << std::endl;
    return 0;
}

int CReceiveSocket::AcceptFromClient()
{
    int len = sizeof(SOCKADDR);
    sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
    return 0;
}

int CReceiveSocket::ReceiveFromClient(char* pData, int size)
{
    recv(sockClient, pData, size, 0);
    return 0;
}

int CReceiveSocket::GetSize(int &w, int &h)
{
    width = w;
    height = h;
    return 0;
}