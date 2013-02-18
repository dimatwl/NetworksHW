//
//  ChatMessage.hpp
//  chat_server
//
//  Created by Dmitriy on 2/15/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_ChatMessage_hpp
#define chat_server_ChatMessage_hpp

#include <vector>
#include <string>
#include <math.h>

#include <boost/cstdint.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "Message.hpp"

namespace ChatServerNamespace {
    
using std::vector;
using std::string;
using boost::uint8_t;
using boost::uint16_t;
using boost::posix_time::ptime;
using boost::posix_time::microsec_clock;
    
class ChatMessage : public Message {
    ptime timeRecieved;
    string content;
    
public:
    
    ChatMessage(string content) : timeRecieved(microsec_clock::universal_time()), content(content) {
        if (content.size() >= pow(2, 16)) {
            content = string(content, 0, pow(2, 16) - 1);
        }
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        uint16_t sizeIn16bits = content.size();
        uint8_t highPart = (sizeIn16bits & 0xFF00) >> 8;
        uint8_t lowPart = sizeIn16bits & 0x00FF;
        toSend.push_back(highPart);
        toSend.push_back(lowPart);
        std::copy(content.begin(), content.end(), back_inserter(toSend));
        return toSend;
    }
    
    ptime getTime() {
        return timeRecieved;
    }
};
    
}

#endif
