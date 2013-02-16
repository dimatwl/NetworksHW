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

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "Counterpart.hpp"
#include "ParticipantResponse.hpp"
#include "CounterpartMessage.hpp"
#include "ChatMessage.hpp"

namespace ChatServerNamespace {
    
using std::set;
using std::vector;

using boost::shared_ptr;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::bind;
using boost::system::error_code;
    
class Participant;

class ChatServer {
    set<shared_ptr<Participant> > participants;
    set<shared_ptr<Counterpart> > counterparts;
    vector<ChatMessage> globalMessageQueue;
    vector<CounterpartMessage> localMessageQueue;
    
    io_service& ioService;
    tcp::acceptor participantsAcceptor;
    tcp::acceptor counterpartsAcceptor;
    
    void startAccept();
    
    void acceptHandler(shared_ptr<Participant> participant, const error_code& error);
public:
    
    ChatServer(io_service& ioService, const tcp::endpoint& participantsEndpoint, const tcp::endpoint& counterpartsEndpoint) : ioService(ioService),
                                                                                                                              participantsAcceptor(ioService, participantsEndpoint),
                                                                                                                              counterpartsAcceptor(ioService, counterpartsEndpoint) {
        startAccept();
    }
    
    void removeParticipant(shared_ptr<Participant> toRemove) {
        participants.erase(toRemove);
    }
    
    void putToGlobalQueue(ChatMessage message) {
        globalMessageQueue.push_back(message);
    }
    
    vector<ChatMessage> getMessagesToSend(size_t nextSendMessageNumber) {
        if (nextSendMessageNumber < globalMessageQueue.size()) {
            return vector<ChatMessage>(globalMessageQueue.begin() + nextSendMessageNumber, globalMessageQueue.end());
        } else {
            return vector<ChatMessage>();
        }
    }
};
    
}

#include "Participant.hpp"


namespace ChatServerNamespace {

void ChatServer::startAccept() {
    shared_ptr<Participant> newParticipant(new Participant(ioService, *this));
    participantsAcceptor.async_accept(newParticipant->getSocket(), bind(
                                                                     &ChatServer::acceptHandler,
                                                                     this,
                                                                     newParticipant,
                                                                     boost::asio::placeholders::error));
}

void ChatServer::acceptHandler(shared_ptr<Participant> participant, const error_code& error) {
    if (!error) {
        participants.insert(participant);
        participant->start();
    }
    startAccept();
}
    
}


using ChatServerNamespace::ChatServer;


#endif
