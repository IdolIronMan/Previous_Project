#include "chat_session.hpp"
#include <unistd.h>
#include <memory>
#include <iostream>

//引用传参,使用当前的room_绑定到服务器的聊天室
ChatSession::ChatSession(int sockfd, ChatRoom& room) :sockfd_(sockfd), room_(room) {}

//析构函数,关闭当前会话
ChatSession::~ChatSession()
{
    std::cout << "~ChatSession()" << std::endl;
    close(sockfd_);
}

//开始(线程函数)
void ChatSession::start()
{
	//把当前会话加入到服务器的聊天室
    room_.join(shared_from_this());
    
    while (true)
    {
    	//不断读取消息并且转发消息,读取失败时,表示离开了聊天室
        int ret = this->read(sockfd_, read_msg_.data(), ChatMessage::header_length);
        if (ret > 0)
        {
        	//解析读到的消息头
            read_msg_.decode_header();
            //读取消息内容
            this->read(sockfd_, read_msg_.body(), read_msg_.body_length());
            //把读取到的消息转发至所有的客户端
            room_.deliver_message(read_msg_);
        }
        else //如果读取失败,就说明客户端主动断开连接
        {
        	//把当前客户端踢出聊天室
            room_.leave(shared_from_this());
            break;
        }
    }
}

//传递消息,把当前消息发送到指定的socket
void ChatSession::deliver_message(const ChatMessage& msg) const 
{
    this->write(sockfd_, msg.data(), msg.length());
}



//包裹函数,读和写
ssize_t ChatSession::read(int fd, char* buf, int size)
{
    int readed_bytes = 0;
    while (true)
    {
        int ret = ::read(fd, buf + readed_bytes, size - readed_bytes);
        if (ret <= 0)
            return ret;
        readed_bytes += ret;
        if (readed_bytes == size)
            break;
    }
    return readed_bytes;
}

ssize_t ChatSession::write(int fd, const char* buf, int size) const
{
    int written_bytes = 0;
    while (true)
    {
        int ret = ::write(fd, buf + written_bytes, size - written_bytes);
        if (ret <= 0)
            return ret;
        written_bytes += ret;
        if (written_bytes == size)
            break;
    }
    return written_bytes;
}



