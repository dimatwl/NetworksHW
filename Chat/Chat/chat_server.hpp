//
//  chat_server.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef Chat_chat_server_h
#define Chat_chat_server_h

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
#include "chat_room.hpp"
#include "chat_participant.hpp"

using boost::asio::ip::tcp;
typedef std::deque<chat_message> chat_message_queue;

class chat_server
{
public:
    
    typedef boost::shared_ptr<chat_server> chat_server_ptr;
    typedef std::list<chat_server_ptr> chat_server_list;
    
    chat_server(boost::asio::io_service& io_service,
                const tcp::endpoint& endpoint)
    : io_service_(io_service),
    acceptor_(io_service, endpoint)
    {
        start_accept();
    }
    
    void start_accept()
    {
        chat_session_ptr new_session(new chat_session(io_service_, room_));
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&chat_server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }
    
    void handle_accept(chat_session_ptr session,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            session->start();
        }
        
        start_accept();
    }
    
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    chat_room room_;
};

typedef boost::shared_ptr<chat_server> chat_server_ptr;
typedef std::list<chat_server_ptr> chat_server_list;

#endif
