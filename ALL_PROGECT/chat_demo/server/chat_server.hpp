#ifndef CHAT_SERVER_HPP_
#define CHAT_SERVER_HPP_

#include "tcp_acceptor.hpp"
#include "chat_room.hpp"

//聊天服务器类
class ChatServer
{
public:
	//构造函数
    ChatServer(short port);
    //服务器运行
    void run();

private:
	//接收TCP连接请求,把接收到的请求加入到聊天室
    TcpAcceptor acceptor_;
    //聊天室
    ChatRoom room_;
};

#endif
