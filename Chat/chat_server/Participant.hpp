//
//  chat_participant.hpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_Participant_hpp
#define chat_server_Participant_hpp

#include <vector>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "ParticipantResponse.hpp"
#include "ChatServer.hpp"
#include "ChatMessage.hpp"

namespace ChatServerNamespace {
    
using std::vector;
    
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::asio::buffer;
using boost::asio::async_read;
using boost::bind;
using boost::uint8_t;
using boost::system::error_code;
    
    
class Participant : public boost::enable_shared_from_this<Participant> {
    tcp::socket socket;
    vector<uint8_t> lastRead;
    ChatServer& server;
    string name;
    size_t nextSendMessageNumber;
    
    static const uint8_t NAME_SIZE_SIZE = 1;
    static const uint8_t MESSAGE_SIZE_SIZE = 2;
    static const uint8_t OP_CODE_SIZE = 1;
    
    static const uint8_t LOGIN_OP_CODE = 1;
    static const uint8_t SEND_OP_CODE = 2;
    static const uint8_t RECIEVE_OP_CODE = 3;
    
    void readOpCodeHandler(const error_code& error) {
        if (!error && lastRead.size() == OP_CODE_SIZE) {
            if (lastRead[0] == LOGIN_OP_CODE) {
                lastRead.resize(NAME_SIZE_SIZE);
                async_read(socket, buffer(lastRead, NAME_SIZE_SIZE), bind(
                                                             &Participant::readNameSizeHandler, shared_from_this(),
                                                             boost::asio::placeholders::error));
            } else if (lastRead[0] == SEND_OP_CODE) {
                lastRead.resize(MESSAGE_SIZE_SIZE);
                async_read(socket, buffer(lastRead, MESSAGE_SIZE_SIZE), bind(
                                                             &Participant::readMessageSizeHandler, shared_from_this(),
                                                             boost::asio::placeholders::error));
            } else if (lastRead[0] == RECIEVE_OP_CODE){
                sendParticipantRecieveResponse();
                lastRead.resize(OP_CODE_SIZE);
                async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                        &Participant::readOpCodeHandler, shared_from_this(),
                                                                        boost::asio::placeholders::error));
            }
        } else {
            server.removeParticipant(shared_from_this());
        }
    }
    
    void readNameSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == NAME_SIZE_SIZE) {
            uint8_t nameSize = lastRead[0];
            if (nameSize > 0) {
                lastRead.resize(nameSize);
                async_read(socket, buffer(lastRead, nameSize), bind(
                                                         &Participant::readNameHandler, shared_from_this(),
                                                         nameSize,
                                                         boost::asio::placeholders::error));
            } else {
                sendParticipantLoginResponse(1, 0);
                lastRead.resize(OP_CODE_SIZE);
                async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                        &Participant::readOpCodeHandler, shared_from_this(),
                                                                        boost::asio::placeholders::error));
            }
        } else {
            server.removeParticipant(shared_from_this());
        }
    }
    
    void readNameHandler(uint8_t nameSize, const error_code& error) {
        if (!error && lastRead.size() == nameSize) {
            name = string(lastRead.begin(), lastRead.end());
            sendParticipantLoginResponse(0, 0);
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Participant::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeParticipant(shared_from_this());
        }
    }
    
    void readMessageSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == MESSAGE_SIZE_SIZE) {
            uint16_t messageSize = (static_cast<uint16_t>(lastRead[0]) << 8) + static_cast<uint16_t>(lastRead[1]);
            if (messageSize > 0) {
                lastRead.resize(messageSize);
                async_read(socket, buffer(lastRead, messageSize), bind(
                                                                    &Participant::readMessageHandler, shared_from_this(),
                                                                    messageSize,
                                                                    boost::asio::placeholders::error));
            }
        } else {
            server.removeParticipant(shared_from_this());
        }

    }
    
    void readMessageHandler(uint16_t messageSize, const error_code& error) {
        if (!error && lastRead.size() == messageSize) {
            string messageString = string(lastRead.begin(), lastRead.end());
            if (!name.empty()) {
                messageString = name + ": " + messageString;
            }
            ChatMessage message(messageString);
            server.putToGlobalQueue(message);
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Participant::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeParticipant(shared_from_this());
        }
    }
    
    void writeHandler(const error_code& error) {
        if (error) {
            server.removeParticipant(shared_from_this());
        }
    }

    
    void sendParticipantLoginResponse(uint8_t statusCode, uint8_t protocolVersion) {
        ParticipantLoginResponse response(statusCode, protocolVersion);
        vector<uint8_t> serializedResponse = response.prepareToSend();
        boost::asio::async_write(socket, buffer(serializedResponse), bind(
                                                                          &Participant::writeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
    }
    
    void sendParticipantRecieveResponse() {
        vector<ChatMessage> mesagesToSend = server.getMessagesToSend(nextSendMessageNumber);
        nextSendMessageNumber += mesagesToSend.size();
        ParticipantRecieveResponse response(mesagesToSend);
        vector<uint8_t> serializedResponse = response.prepareToSend();
        boost::asio::async_write(socket, buffer(serializedResponse), bind(
                                                                          &Participant::writeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
    }
    
        
public:
    Participant(io_service& ioService, ChatServer& server) : socket(ioService), server(server), name(), nextSendMessageNumber(0) {
        
    }
    
    void start() {
        lastRead.resize(OP_CODE_SIZE);
        async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                            &Participant::readOpCodeHandler, shared_from_this(),
                                            boost::asio::placeholders::error));
    }
    
    tcp::socket& getSocket() {
        return socket;
    }
    
    
};
    
}


#endif
