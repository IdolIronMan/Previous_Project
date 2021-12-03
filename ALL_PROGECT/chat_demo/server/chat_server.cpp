#include "chat_server.hpp"
#include <thread>
#include <iostream>
#include "chat_message.hpp"
#include "chat_session.hpp"
using namespace std;

//构造函数,构造函数初始化列表
ChatServer::ChatServer(short port): acceptor_(port)
{
    cout << "server init success!!!" << endl;
}

//开始运行,当有客户端连接的时候,就开始一个分离的线程开始一个会话,加入当前房间
void ChatServer::run()
{
    while (true)
    {
        // 接收客户要的连接,返回和客户端通信的连接套接字
        int confd = acceptor_.accept();
        cout << "confd = " << confd << endl;
        //根据客户端的连接套接字创建一个会话,把这个会话加入聊天室
        //当会话结束的时候,可以自动的回收资源
        //创建的会话是属于某一个聊天室(属于当前服务器的聊天室)
        std::thread(&ChatSession::start,std::make_shared<ChatSession>(confd, room_)).detach();
    }
}



