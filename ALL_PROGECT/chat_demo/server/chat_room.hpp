#ifndef CHAT_ROOM_HPP_
#define CHAT_ROOM_HPP_

#include <set>
#include <mutex>
#include "chat_participant.hpp"
#include "chat_message.hpp"

//聊天室类
class ChatRoom
{
public:
	//加入聊天室
    void join(ChatParticipantPtr participant);
    
    //离开聊天室
    void leave(ChatParticipantPtr participant);
    
    //传递消息
    void deliver_message(const ChatMessage& msg);
    
private:
	//集合保存所有聊天者的基类指针
    std::set<ChatParticipantPtr> participants_;
    std::mutex mutex_;
};

#endif