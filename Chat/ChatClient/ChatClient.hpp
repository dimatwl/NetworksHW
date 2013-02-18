//
//  chat_server.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef Chat_server_ChatServer_hpp
#define Chat_server_ChatServer_hpp

#include <set>
#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "ParticipantRequest.hpp"
#include "ChatMessage.hpp"

namespace ChatClientNamespace {
    
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::bind;
using boost::system::error_code;
using boost::asio::async_connect;
using boost::asio::buffer;
using boost::uint8_t;
using boost::asio::deadline_timer;
using boost::posix_time::milliseconds;
    
using std::string;
using std::cout;
using std::endl;
    

class ChatClient {
    io_service& ioService;
    tcp::socket socket;
    string name;
    deadline_timer recieveTimer;
    vector<uint8_t> lastRead;
    
    static const uint8_t PROTOCOL_VERSION = 0;
    static const uint8_t LOGIN_RESPONSE_SIZE = 2;
    static const uint8_t MESSAGE_SIZE_SIZE = 2;
    
    void connectHandler(const boost::system::error_code& error) {
        if (!error) {
            //Send login reques.
            ParticipantLoginRequest request(name);
            vector<uint8_t> serializedRequest = request.prepareToSend();
            async_write(socket, buffer(serializedRequest), bind(
                                                                              &ChatClient::writeLoginHandler,
                                                                              this,
                                                                              boost::asio::placeholders::error));
        } else {
            std::cerr << "Can't connect to server due to error." << error.message() << endl;
        }
    }
    
    void writeLoginHandler(const boost::system::error_code& error) {
        if (!error) {
            //Read login response.
            lastRead.resize(LOGIN_RESPONSE_SIZE);
            async_read(socket, buffer(lastRead, LOGIN_RESPONSE_SIZE), bind(
                                                                      &ChatClient::readLoginHandler,
                                                                      this,
                                                                      boost::asio::placeholders::error));
        }
        else {
            std::cerr << "Can't send login to server due to error." << error.message() << endl;
        }
    }
    
    void readLoginHandler(const boost::system::error_code& error) {
        if (!error && lastRead.size() == LOGIN_RESPONSE_SIZE && !lastRead[0] && lastRead[1] <= PROTOCOL_VERSION) {
            sendRecieve();
        } else {
            std::cerr << "Can't recieve login response from server due to error." << error.message() << endl;
        }
    }
    
    void sendRecieve() {
        ParticipantRecieveRequest request;
        vector<uint8_t> serializedRequest = request.prepareToSend();
        async_write(socket, buffer(serializedRequest), bind(
                                                                         &ChatClient::writeRecieveHandler,
                                                                         this,
                                                                         boost::asio::placeholders::error));
    }
    
    void writeRecieveHandler(const boost::system::error_code& error) {
        if (!error) {
            //Read next message size.
            lastRead.resize(MESSAGE_SIZE_SIZE);
            async_read(socket, buffer(lastRead, MESSAGE_SIZE_SIZE), bind(
                                                                           &ChatClient::readMessageSizeHandler,
                                                                           this,
                                                                           boost::asio::placeholders::error));
        } else {
            std::cerr << "Can't send recieve request to server due to error." << error.message() << endl;
        }
    }
    
    void readMessageSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == MESSAGE_SIZE_SIZE) {
            uint16_t messageSize = (static_cast<uint16_t>(lastRead[0]) << 8) + static_cast<uint16_t>(lastRead[1]);
            if (messageSize > 0) {
                //Read message body.
                lastRead.resize(messageSize);
                async_read(socket, buffer(lastRead, messageSize), bind(
                                                                       &ChatClient::readMessageHandler,
                                                                       this,
                                                                       messageSize,
                                                                       boost::asio::placeholders::error));
            } else {
                recieveTimer.expires_from_now(milliseconds(100));
                recieveTimer.async_wait(bind(
                                             &ChatClient::sendRecieve,
                                             this));
            }
        } else {
            std::cerr << "Can't recieve mesage from server due to error." << error.message() << endl;
        }
        
    }
    
    void readMessageHandler(uint16_t messageSize, const error_code& error) {
        if (!error && lastRead.size() == messageSize) {
            string messageString = string(lastRead.begin(), lastRead.end());
            ChatMessage message(messageString);
            print(message);
            //Read next message size.
            lastRead.resize(MESSAGE_SIZE_SIZE);
            async_read(socket, buffer(lastRead, MESSAGE_SIZE_SIZE), bind(
                                                                         &ChatClient::readMessageSizeHandler,
                                                                         this,
                                                                         boost::asio::placeholders::error));
        } else {
            std::cerr << "Can't recieve mesage from server due to error." << error.message() << endl;
        }
    }
    
    void writeMessageHandler(const boost::system::error_code& error) {
        if (error) {
            std::cerr << "Last message wasn't sent due to error." << error.message() << endl;
        }
    }
    
    void doClose() {
        socket.close();
    }
    
    void doWrite(ChatMessage message) {
        ParticipantSendRequest request(message);
        vector<uint8_t> serializedRequest = request.prepareToSend();
        async_write(socket, buffer(serializedRequest), bind(
                                                                         &ChatClient::writeMessageHandler,
                                                                         this,
                                                                         boost::asio::placeholders::error));
    }
    
    void print(ChatMessage message) {
        cout << message.getContent() << endl;
    }
    
    
public:
    ChatClient(io_service& ioService, tcp::resolver::iterator endpointIterator, string name) : ioService(ioService), socket(ioService), name(name), recieveTimer(ioService) {
        async_connect(socket, endpointIterator,
                                   bind(&ChatClient::connectHandler,
                                        this,
                                        boost::asio::placeholders::error));
    }
    
    void close () {
        ioService.post(bind(&ChatClient::doClose, this));
    }
    
    void send(ChatMessage message) {
        ioService.post(bind(&ChatClient::doWrite, this, message));
    }
    
};
    
}


using ChatClientNamespace::ChatClient;


#endif
