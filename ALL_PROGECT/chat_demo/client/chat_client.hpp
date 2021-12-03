#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP


#include <string>

//客户端只需要连接服务器,能发送消息,能接收消息
class ChatClient
{
public:
	//构造函数(连上服务器)
    ChatClient(std::string ip, short port);
	//析构函数
    ~ChatClient();
	//运行函数
    void run();

private:
	//接收消息
    void recv_message();
	//发送消息
    void send_message();

    ssize_t read(int fd, char* buf, int size);
    ssize_t write(int fd, const char* buf, int size);
private:
	//套接字描述符
    int sockfd_;
	//客户名称
	//等级
	//vip
	//....
};

#endif