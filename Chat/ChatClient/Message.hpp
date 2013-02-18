//
//  Message.hpp
//  chat_server
//
//  Created by Dmitriy on 2/15/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_Message_hpp
#define chat_server_Message_hpp

#include <vector>

#include <boost/cstdint.hpp>

namespace ChatClientNamespace {
    
using std::vector;
using boost::uint8_t;

class Message {
public:
    virtual vector<uint8_t> prepareToSend() const = 0;
    virtual ~Message(){}
};
    
}

#endif
