//
//  chat_participant.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef Chat_chat_participant_h
#define Chat_chat_participant_h


class chat_participant
{
public:

    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};


#endif
