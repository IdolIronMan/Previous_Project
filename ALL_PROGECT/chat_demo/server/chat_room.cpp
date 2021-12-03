#include "chat_room.hpp"

//使用互斥锁管理者
//加入聊天室
void ChatRoom::join(ChatParticipantPtr participant)
{
    std::lock_guard<std::mutex> lg(mutex_);
    this->participants_.insert(participant);
}
//离开聊天室
void ChatRoom::leave(ChatParticipantPtr participant)
{
    std::lock_guard<std::mutex> lg(mutex_);
    //因为使用的是智能指针,所有擦除聊天者时会自动的释放前面开辟的动态空间
    this->participants_.erase(participant);
}

//传递消息,遍历整个集合,让集合中的每一个聊天者都转发一次msg表示的消息
void ChatRoom::deliver_message(const ChatMessage& msg)
{
    std::lock_guard<std::mutex> lg(mutex_);
    for (auto& p: participants_)
        p->deliver_message(msg);
}