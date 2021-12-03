#include "chat_client.hpp"
#include "chat_message.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <iostream>
#include <thread>
using namespace std;

//构造函数
ChatClient::ChatClient(std::string ip, short port)
{
    //创建一个socket
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM-tcp协议，SOCK-DTRAM-udp协议
    if (sockfd_ == -1)
        throw std::runtime_error("socket error");

    //描述服务器地址,绑定网络信息结构体
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));          //初始化
    addr.sin_family = AF_INET;                    //ipv4协议
    addr.sin_port = htons(port);                  //将主机字节序转换为网络字节序，s是short
    addr.sin_addr.s_addr = inet_addr(ip.c_str()); //将字符串string类型转化为char*类型
    //连接服务器
    if (connect(sockfd_, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("connect server error!");

    cout << "connect server success!!" << endl;
}
//析构函数
ChatClient::~ChatClient()
{
    close(sockfd_);
}

//运行函数,两个线程,一个读,一个写
void ChatClient::run()
{
    //通过线程函数
    //一个线程读消息(接收)
    std::thread t1(&ChatClient::recv_message); //thread t1(成员函数，函数参数);
    //一个线程发送消息(监控标准输入)
    std::thread t2(&ChatClient::send_message); //第二个参数可以是this
    t1.join();
    t2.join();
}

//读取消息
void ChatClient::recv_message()
{
    ChatMessage msg;
    while (true)
    {
        //使用自定义的包裹函数读取消息的头部
        int ret = this->read(sockfd_, msg.data(), ChatMessage::header_length);
        if (ret <= 0) //body_len是0则没有消息，退出循环
            break;
        //解析消息头
        msg.decode_header();
        //根据消息头部读取指定的字节数
        this->read(sockfd_, msg.body(), msg.body_length());
        cout << "->" << msg.body() << endl;
        msg.clear();
    }
}

//自定义读函数,从fd中读取size个字节存放到buf
//包裹思想,保证读到size个字节,返回实际读取到的字节数
ssize_t ChatClient::read(int fd, char *buf, int size)
{
    int readed_bytes = 0;
    //不停的读取fd中的内容
    while (true)
    {
        //封装了一个全局的read函数
        int ret = ::read(fd, buf + readed_bytes, size - readed_bytes);
        if (ret <= 0)
            return ret;
        readed_bytes += ret;
        if (readed_bytes == size)
            break;
    }
    return readed_bytes;
}

//发送消息
void ChatClient::send_message()
{
    ChatMessage msg;
    while (true)
    {
        //从标准输入获取信息
        std::cin.getline(msg.body(), ChatMessage::body_max_length);
        //设置消息长度
        msg.set_body_length(std::strlen(msg.body()));
        msg.encode_header();
        //使用自定义的发送函数发送指定消息
        int ret = this->write(sockfd_, msg.data(), msg.length());
        if (ret <= 0)
            break;
    }
}

//自定义的写入函数,写入指定个字节,返回实际写入的字节数
ssize_t ChatClient::write(int fd, const char *buf, int size)
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
