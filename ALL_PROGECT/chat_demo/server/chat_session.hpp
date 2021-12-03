#ifndef CHAT_SESSION_HPP_
#define CHAT_SESSION_HPP_

#include "chat_participant.hpp"
#include "chat_room.hpp"

/*
std::enable_shared_from_this 能让一个对象（假设其名为 t ，且已被一个 std::shared_ptr
对象 pt 管理）安全地生成其他额外的 std::shared_ptr 实例（假设名为 pt1, pt2, ... ） ，
它们与 pt 共享对象 t 的所有权。
若一个类T继承std::enable_shared_from_this<T>,则会为该类T提供成员函数：
shared_from_this.当 T 类型对象t被一个为名为pt的std::shared_ptr<T>类对象管理时，
调用T::shared_from_this成员函数,将会返回一个新的std::shared_ptr<T>对象,它与pt共享
t的所有权。
*/

//聊天会话类,每一个会话都属于一个房间
class ChatSession : public ChatParticipant, public std::enable_shared_from_this<ChatSession>
{
public:
	//构造函数
	//第一个参数是连接套接字,第二个参数的次会话属于哪一个房间
    ChatSession(int sockfd, ChatRoom& room);
    //析构函数
    ~ChatSession();
    //开始
    void start();
    //重写传递消息
    void deliver_message(const ChatMessage& msg) const override;

private:
	//包裹函数,读和写
    ssize_t read(int fd, char* buf, int size);
    ssize_t write(int fd, const char* buf, int size) const;

private:
	//套接字描述符,记录和客户端通信的连接套接字
    int sockfd_;
    //此会话属于哪一个聊天室
    ChatRoom& room_;
    //当前会话接收到的消息(当前客户端发送给服务器的消息)
    ChatMessage read_msg_;
};


#endif


