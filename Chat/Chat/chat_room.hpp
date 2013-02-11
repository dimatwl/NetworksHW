//
//  chat_room.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef Chat_chat_room_h
#define Chat_chat_room_h

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "chat_session.hpp"
#include "chat_participant.hpp"

typedef std::deque<chat_message> chat_message_queue;

class chat_room
{
public:
    void join(chat_participant_ptr participant)
    {
        participants_.insert(participant);
        std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                      boost::bind(&chat_participant::deliver, participant, _1));
    }
    
    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }
    
    void deliver(const chat_message& msg)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();
        
        std::for_each(participants_.begin(), participants_.end(),
                      boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
    }
    
private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

#endif
