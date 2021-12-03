#include "chat_message.hpp"
#include <cstring>
#include <cstdlib>

//获取内容
char *ChatMessage::data()
{
    return data_;
}
const char *ChatMessage::data() const
{
    return data_;
}

//消息长度
size_t ChatMessage::length() const
{
    return header_length + body_length_;
}

//获取消息体
char *ChatMessage::body()
{
    //指针做偏移，偏移4个字节就是消息的内容
    return data_ + header_length;
}
const char *ChatMessage::body() const
{
    return data_ + header_length;
}

//设置消息内容长度
void ChatMessage::set_body_length(size_t len)
{
    body_length_ = len;
    if (body_length_ > body_max_length)
    {
        body_length_ = 0; //超过最长的长度就置为0
    }
}

//获取消息体长度
size_t ChatMessage::body_length() const
{
    return body_length_;
}

//封装头部，把消息体长度写入字节数组
void ChatMessage::encode_header()
{
    char header[header_length + 1] = " ";
    //将size_t转化成int类型，以四位填充
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, header_length);
}

//解析头部信息,从字节数组中解析消息体的长度
void ChatMessage::decode_header()
{
    char header[header_length + 1] = "";
    std::strncpy(header, data_, header_length);
    body_length_ = std::atoi(header);
    if (body_length_ > body_max_length)
        body_length_ = 0;
}

void ChatMessage::clear()
{
    std::memset(data_, 0, sizeof(data_));
}
