#ifndef CHAT_PARTICIPANT_HPP_
#define CHAT_PARTICIPANT_HPP_

#include <memory>
#include "chat_message.hpp"

//聊天参加者
//抽象类
class ChatParticipant
{
public:
	//虚析构函数
    virtual ~ChatParticipant() = default;
	//纯虚函数,传递消息
    virtual void deliver_message(const ChatMessage&) const = 0;
};

using ChatParticipantPtr = std::shared_ptr<ChatParticipant>;

#endif