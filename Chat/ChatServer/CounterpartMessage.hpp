//
//  CounterpartMessage.hpp
//  chat_server
//
//  Created by Dmitriy on 2/13/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_CounterpartMessage_hpp
#define chat_server_CounterpartMessage_hpp

#include <vector>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/cstdint.hpp>

namespace ChatServerNamespace {
    
using std::vector;
using std::string;
    
using boost::posix_time::ptime;
    using boost::posix_time::to_iso_string;
using boost::posix_time::time_duration;
using boost::posix_time::microsec_clock;
using boost::uint8_t;

class CounterpartMessage {
public:
    CounterpartMessage(){
        
    }
    virtual ~CounterpartMessage(){}
};
    
class GetTimeRequest : CounterpartMessage {
    ptime hostTime;
    
public:
    
    GetTimeRequest() : hostTime(microsec_clock::universal_time()) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(getOpCode());
        string timeString = to_iso_string(hostTime);
        uint8_t stringSize = static_cast<uint8_t>(timeString.size());
        toSend.push_back(stringSize);
        std::copy(timeString.begin(), timeString.end(), back_inserter(toSend));
        return toSend;
    }
    
    static uint8_t getOpCode() {
        return 0;
    }
    
};
    
class GetTimeResponse : CounterpartMessage {
    ptime hostTime;
    ptime counterpartTime;
    
public:
    
    GetTimeResponse(ptime hostTime) : hostTime(hostTime), counterpartTime(microsec_clock::universal_time()) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(getOpCode());
        string timeString = to_iso_string(hostTime);
        uint8_t stringSize = static_cast<uint8_t>(timeString.size());
        toSend.push_back(stringSize);
        std::copy(timeString.begin(), timeString.end(), back_inserter(toSend));
        
        timeString = to_iso_string(counterpartTime);
        stringSize = static_cast<uint8_t>(timeString.size());
        toSend.push_back(stringSize);
        std::copy(timeString.begin(), timeString.end(), back_inserter(toSend));
        return toSend;
    }
    
    static uint8_t getOpCode() {
        return 1;
    }
    
};
    
class EarliestMessageRequest : CounterpartMessage {
    ptime earliestMessageTime;
    
public:
    
    EarliestMessageRequest(ptime earliestMessageTime) : earliestMessageTime(earliestMessageTime) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(getOpCode());
        string timeString = to_iso_string(earliestMessageTime);
        uint8_t stringSize = static_cast<uint8_t>(timeString.size());
        toSend.push_back(stringSize);
        std::copy(timeString.begin(), timeString.end(), back_inserter(toSend));
        return toSend;
    }
    
    static uint8_t getOpCode() {
        return 2;
    }
    
};
    
class EarliestMessageResponse : CounterpartMessage {
    bool result;
public:
    
    EarliestMessageResponse(bool result) : result(result) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(getOpCode());
        toSend.push_back(static_cast<uint8_t>(result));
        return toSend;
    }
    
    static uint8_t getOpCode() {
        return 3;
    }
    
};
    
class DeliverMessageRequest : CounterpartMessage {
    ChatMessage message;
    
public:
    
    DeliverMessageRequest(ChatMessage message) : message(message) {
        
    }
    
    vector<uint8_t> prepareToSend() const {
        vector<uint8_t> toSend;
        toSend.push_back(getOpCode());
        vector<uint8_t> preparedToSend = message.prepareToSend();
        toSend.insert(toSend.end(), preparedToSend.begin(), preparedToSend.end());
        return toSend;
    }
    
    static uint8_t getOpCode() {
        return 4;
    }
    
};

    
}

#endif
