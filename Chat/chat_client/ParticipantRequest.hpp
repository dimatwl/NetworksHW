//
//  chat_message.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_ParticipantRespose_hpp
#define chat_server_ParticipantRespose_hpp

#include <vector>
#include <string>
#include <math.h>

#include <boost/cstdint.hpp>

#include "Message.hpp"
#include "ChatMessage.hpp"

namespace ChatClientNamespace {
    
using std::vector;
using std::string;
    
using boost::uint8_t;

class ParticipantRequest : public Message {
public:
    virtual ~ParticipantRequest(){}
};
    
    
class ParticipantLoginRequest : public ParticipantRequest {
    const string name;
    const uint8_t OP_CODE;

public:
    ParticipantLoginRequest(string name) : name(name), OP_CODE(1) {
        if (name.size() >= pow(2, 8)) {
            name = string(name, 0, pow(2, 8) - 1);
        }
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(OP_CODE);
        toSend.push_back(static_cast<uint8_t>(name.size()));
        std::copy(name.begin(), name.end(), back_inserter(toSend));
        return toSend;
    }
};
    
class ParticipantSendRequest : public ParticipantRequest {
    const ChatMessage message;
    const uint8_t OP_CODE;
    
public:
    ParticipantSendRequest(ChatMessage message) : message(message), OP_CODE(2) {

    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(OP_CODE);
        vector<uint8_t> serializedMessage = message.prepareToSend();
        std::copy(serializedMessage.begin(), serializedMessage.end(), back_inserter(toSend));
        return toSend;
    }
};
    
    
class ParticipantRecieveRequest : public ParticipantRequest {
    const uint8_t OP_CODE;
    
public:
    ParticipantRecieveRequest() : OP_CODE(3){
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(OP_CODE);
        return toSend;
    }
};
    
}

#endif
