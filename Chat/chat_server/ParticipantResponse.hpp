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

#include <boost/cstdint.hpp>

#include "Message.hpp"
#include "ChatMessage.hpp"

namespace ChatServerNamespace {
    
using std::vector;
using boost::uint8_t;

class ParticipantResponse : public Message {
public:
    virtual ~ParticipantResponse(){}
};
    
    
class ParticipantLoginResponse : public ParticipantResponse {
    const uint8_t statusCode;
    const uint8_t protocolVersion;
public:
    ParticipantLoginResponse(uint8_t statusCode, uint8_t protocolVersion) : statusCode(statusCode), protocolVersion(protocolVersion){
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(statusCode);
        toSend.push_back(protocolVersion);
        return toSend;
    }
};
    
    
class ParticipantRecieveResponse : public ParticipantResponse {
    const vector<ChatMessage>& messagesToSend;
    
public:
    ParticipantRecieveResponse(const vector<ChatMessage>& messagesToSend) : messagesToSend(messagesToSend) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        for (vector<ChatMessage>::const_iterator i = messagesToSend.begin(); i != messagesToSend.end(); ++i) {
            vector<uint8_t> preparedToSend = i->prepareToSend();
            toSend.insert(toSend.end(), preparedToSend.begin(), preparedToSend.end());
        }
        toSend.push_back(0x00);
        toSend.push_back(0x00);
        return toSend;
    }
};
    
}

#endif
