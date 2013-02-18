//
//  ChatCounterpart.h
//  chat_server
//
//  Created by Dmitriy on 2/12/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#ifndef chat_server_Counterpart_hpp
#define chat_server_Counterpart_hpp

#include <vector>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/cstdint.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

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
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::posix_time::from_iso_string;

class Counterpart : public boost::enable_shared_from_this<Counterpart> {
    tcp::socket socket;
    ChatServer& server;
    vector<uint8_t> lastRead;
    time_duration timeDifference;
    
    ptime earliestTimeToTest;
    
    static const uint8_t OP_CODE_SIZE = 1;
    static const uint8_t TIME_SIZE_SIZE = 1;
    static const uint8_t BOOL_SIZE = 1;
    static const uint8_t MESSAGE_SIZE_SIZE = 2;
    
    
    void readOpCodeHandler(const error_code& error) {
        if (!error && lastRead.size() == OP_CODE_SIZE) {
            if (lastRead[0] == GetTimeRequest::getOpCode()) {
                lastRead.resize(TIME_SIZE_SIZE);
                async_read(socket, buffer(lastRead, TIME_SIZE_SIZE), bind(
                                                                          &Counterpart::readTimeRequestTimeSizeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
            } else if (lastRead[0] == GetTimeResponse::getOpCode()) {
                lastRead.resize(TIME_SIZE_SIZE);
                async_read(socket, buffer(lastRead, TIME_SIZE_SIZE), bind(
                                                                          &Counterpart::readTimeResponseHostTimeSizeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
            } else if (lastRead[0] == EarliestMessageRequest::getOpCode()) {
                lastRead.resize(TIME_SIZE_SIZE);
                async_read(socket, buffer(lastRead, TIME_SIZE_SIZE), bind(
                                                                          &Counterpart::readEarliestMessageRequestTimeSizeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
            } else if (lastRead[0] == EarliestMessageResponse::getOpCode()) {
                lastRead.resize(BOOL_SIZE);
                async_read(socket, buffer(lastRead, BOOL_SIZE), bind(
                                                                          &Counterpart::readEarliestMessageResponseResultHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
            } else if (lastRead[0] == DeliverMessageRequest::getOpCode()) {
                lastRead.resize(MESSAGE_SIZE_SIZE);
                async_read(socket, buffer(lastRead, MESSAGE_SIZE_SIZE), bind(
                                                                     &Counterpart::readMessageSizeHandler, shared_from_this(),
                                                                     boost::asio::placeholders::error));
            }
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeRequestTimeSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == TIME_SIZE_SIZE) {
            uint8_t timeSize = lastRead[0];
            lastRead.resize(timeSize);
            async_read(socket, buffer(lastRead, timeSize), bind(
                                                                      &Counterpart::readTimeRequestTimeHandler, shared_from_this(),
                                                                      timeSize,
                                                                      boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeResponseHostTimeSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == TIME_SIZE_SIZE) {
            uint8_t timeSize = lastRead[0];
            lastRead.resize(timeSize);
            async_read(socket, buffer(lastRead, timeSize), bind(
                                                                &Counterpart::readTimeResponseHostTimeHandler, shared_from_this(),
                                                                timeSize,
                                                                boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readEarliestMessageRequestTimeSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == TIME_SIZE_SIZE) {
            uint8_t timeSize = lastRead[0];
            lastRead.resize(timeSize);
            async_read(socket, buffer(lastRead, timeSize), bind(
                                                                &Counterpart::readEarliestMessageRequestTimeHandler, shared_from_this(),
                                                                timeSize,
                                                                boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeRequestTimeHandler(uint8_t timeSize, const error_code& error) {
        if (!error && lastRead.size() == timeSize) {
            string timeString = string(lastRead.begin(), lastRead.end());
            ptime hostTime = from_iso_string(timeString);
            
            //IMPORTANT
            //Sending local time with host time.
            GetTimeResponse response(hostTime);
            vector<uint8_t> serializedResponse = response.prepareToSend();
            boost::asio::async_write(socket, buffer(serializedResponse), bind(
                                                                              &Counterpart::writeHandler, shared_from_this(),
                                                                              boost::asio::placeholders::error));
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeResponseHostTimeHandler(uint8_t timeSize, const error_code& error) {
        if (!error && lastRead.size() == timeSize) {
            string timeString = string(lastRead.begin(), lastRead.end());
            ptime hostTime = from_iso_string(timeString);
            lastRead.resize(TIME_SIZE_SIZE);
            async_read(socket, buffer(lastRead, TIME_SIZE_SIZE), bind(
                                                                      &Counterpart::readTimeResponseCounterpartTimeSizeHandler, shared_from_this(),
                                                                      hostTime,
                                                                      boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readEarliestMessageRequestTimeHandler(uint8_t timeSize, const error_code& error) {
        if (!error && lastRead.size() == timeSize) {
            string timeString = string(lastRead.begin(), lastRead.end());
            ptime messageTime = from_iso_string(timeString);
            
            //IMPORTANT
            //Time here is prepared, e.g. it's local.
            bool result = server.haveMessageEarlier(messageTime);
            
            EarliestMessageResponse response(result);
            vector<uint8_t> serializedResponse = response.prepareToSend();
            boost::asio::async_write(socket, buffer(serializedResponse), bind(
                                                                              &Counterpart::writeHandler, shared_from_this(),
                                                                              boost::asio::placeholders::error));
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeResponseCounterpartTimeSizeHandler(ptime hostTime, const error_code& error) {
        if (!error && lastRead.size() == TIME_SIZE_SIZE) {
            uint8_t timeSize = lastRead[0];
            lastRead.resize(timeSize);
            async_read(socket, buffer(lastRead, timeSize), bind(
                                                                &Counterpart::readTimeResponseCounterpartTimeHandler, shared_from_this(),
                                                                timeSize,
                                                                hostTime,
                                                                boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readTimeResponseCounterpartTimeHandler(uint8_t timeSize, ptime hostTime, const error_code& error) {
        if (!error && lastRead.size() == timeSize) {
            string timeString = string(lastRead.begin(), lastRead.end());
            
            //IMPORTANT
            //Got host time when message were sent to counterpart and counterpart time when message were sent to host.
            //So differense between old host time and current time is doubeled channel delay.
            //Current counterpart time is sum of counterpart time when message were sent to host and channel delay.
            ptime counterpartTime = from_iso_string(timeString);
            ptime secondHostTime(microsec_clock::universal_time());
            time_duration channelDelay = (hostTime - secondHostTime) / 2;
            ptime realCounterPartTime = counterpartTime + channelDelay;
            timeDifference = secondHostTime - realCounterPartTime;
            
            //Sending of earliest message time to test if counterpart have something earlier unsent.
            //IMPORTANT
            //Time from host MUST be converted to counterpart's time.
            ptime earliestTimeToTestOnCounterpart = earliestTimeToTest - timeDifference;
            EarliestMessageRequest request(earliestTimeToTestOnCounterpart);
            vector<uint8_t> serializedRequest = request.prepareToSend();
            boost::asio::async_write(socket, buffer(serializedRequest), bind(
                                                                             &Counterpart::writeHandler, shared_from_this(),
                                                                             boost::asio::placeholders::error));
            
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readEarliestMessageResponseResultHandler(const error_code& error) {
        if (!error && lastRead.size() == BOOL_SIZE) {
            bool result = static_cast<bool>(lastRead[0]);
            //IMPORTANT
            //Got result here. Update infomation on server.
            server.updateEarliestMesageResults(shared_from_this(), result);
            
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void readMessageSizeHandler(const error_code& error) {
        if (!error && lastRead.size() == MESSAGE_SIZE_SIZE) {
            uint16_t messageSize = (static_cast<uint16_t>(lastRead[0]) << 8) + static_cast<uint16_t>(lastRead[1]);
            lastRead.resize(messageSize);
            async_read(socket, buffer(lastRead, messageSize), bind(
                                                                       &Counterpart::readMessageHandler, shared_from_this(),
                                                                       messageSize,
                                                                       boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
        
    }
    
    void readMessageHandler(uint16_t messageSize, const error_code& error) {
        if (!error && lastRead.size() == messageSize) {
            string messageString = string(lastRead.begin(), lastRead.end());
            ChatMessage message(messageString);
            server.putToGlobalQueue(message);
            lastRead.resize(OP_CODE_SIZE);
            async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                    &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                    boost::asio::placeholders::error));
        } else {
            server.removeCounterpart(shared_from_this());
        }
    }
    
    void writeHandler(const error_code& error) {
        if (error) {
            server.removeCounterpart(shared_from_this());
        }
    }

public:
    Counterpart(io_service& ioService, ChatServer& server) : socket(ioService), server(server) {
        
    }
    
    void start() {
        lastRead.resize(OP_CODE_SIZE);
        async_read(socket, buffer(lastRead, OP_CODE_SIZE), bind(
                                                                &Counterpart::readOpCodeHandler, shared_from_this(),
                                                                boost::asio::placeholders::error));
    }
    
    tcp::socket& getSocket() {
        return socket;
    }
    
    void deliverMessage(ChatMessage message) {
        DeliverMessageRequest request(message);
        vector<uint8_t> serializedRequest = request.prepareToSend();
        boost::asio::async_write(socket, buffer(serializedRequest), bind(
                                                                          &Counterpart::writeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
    }
    
    void tryEarliest(ptime earliestTime) {
        earliestTimeToTest = earliestTime;
        GetTimeRequest request;
        vector<uint8_t> serializedRequest = request.prepareToSend();
        boost::asio::async_write(socket, buffer(serializedRequest), bind(
                                                                          &Counterpart::writeHandler, shared_from_this(),
                                                                          boost::asio::placeholders::error));
    }

};
    
}

#endif
