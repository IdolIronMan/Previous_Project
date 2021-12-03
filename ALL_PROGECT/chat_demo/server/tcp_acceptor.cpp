#include "tcp_acceptor.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <memory>
#include <cstring>

//TCP服务器
TcpAcceptor::TcpAcceptor(short port)
{
	//创建TCP套接字
    sockfd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ == -1)
        throw std::runtime_error("socket error");
    
    // 允许地址重用
    int value = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
        throw std::runtime_error("set socket opt error");

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
	//绑定IP地址
    if (bind(sockfd_, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("bind error");
    //开启监听
    if (listen(sockfd_, 10) == -1)
        throw std::runtime_error("listen error");

}
TcpAcceptor::~TcpAcceptor()
{
    ::close(sockfd_);
}
//接收客户端连接(返回用于和客户端通信的连接套接字)
int TcpAcceptor::accept()
{
    int confd  = ::accept(sockfd_, NULL, NULL);
    if (confd == -1)
        throw std::runtime_error("accept error");
    return confd;
}