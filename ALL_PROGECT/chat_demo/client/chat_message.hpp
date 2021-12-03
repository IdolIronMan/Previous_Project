#ifndef CHAT_MESSAGE_HPP_
#define CHAT_MESSAGE_HPP_

/*
消息由两部分组成：
    消息头(header) 4->消息体的长度
	//还可以增加一个字段表示该消息是群聊还是私聊
    消息体(body)
    
*/
#include <cstdio>
//定义一个消息类
class ChatMessage
{
public:
    //枚举相当于常量
    enum
    {
        header_length = 4,
        body_max_length = 1024
    };

    //获取内容
    char *data();
    const char *data() const;
    //消息长度
    size_t length() const;

    char *body();
    const char *body() const;

    //设置消息内容长度
    void set_body_length(size_t len);
    //获取内容长度
    size_t body_length() const;

    //封装头部，把消息体长度写入字节数组
    void encode_header();

    //解析头部信息，从字节数组中解析消息体的长度
    void decode_header();
    void clear();

private:
    //消息内容
    char data_[header_length + body_max_length];
    //消息体长度
    size_t body_length_;
};

#endif