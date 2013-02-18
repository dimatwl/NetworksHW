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
#include <map>
#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "ParticipantResponse.hpp"
#include "CounterpartMessage.hpp"
#include "ChatMessage.hpp"

namespace ChatServerNamespace {
    
using std::set;
using std::vector;
using std::map;
using std::make_pair;
using std::queue;
    
using boost::shared_ptr;
using boost::asio::ip::tcp;
using boost::asio::io_service;
using boost::bind;
using boost::system::error_code;
using boost::posix_time::ptime;
using boost::posix_time::time_duration;
using boost::asio::deadline_timer;
using boost::posix_time::milliseconds;
    
class Participant;
class Counterpart;

class ChatServer {
    set<shared_ptr<Participant> > participants;
    set<shared_ptr<Counterpart> > counterparts;
    vector<ChatMessage> globalMessageQueue;
    queue<ChatMessage> localMessageQueue;
    
    map<shared_ptr<Counterpart>, bool> lastMessageEarliest;
    
    io_service& ioService;
    tcp::acceptor participantsAcceptor;
    tcp::acceptor counterpartsAcceptor;
    
    deadline_timer messageSendTimer;
    
    void startAcceptParticipants();
    void acceptParticipantHandler(shared_ptr<Participant> participant, const error_code& error);
    
    void startAcceptCounterparts();
    void acceptCounterpartHandler(shared_ptr<Counterpart> counterpart, const error_code& error);
    
    void connectKnownCounterparts(const vector<tcp::resolver::iterator>& knownCounterparts);
    
    set<shared_ptr<Counterpart> > getRespondedCounterparts() {
        set<shared_ptr<Counterpart> > result;
        for (map<shared_ptr<Counterpart>, bool>::const_iterator i = lastMessageEarliest.begin(); i != lastMessageEarliest.end(); ++i) {
            result.insert(i->first);
        }
        return result;
    }
    
    void deliverEarliestMessage();
    
    void checkEarliestMessageStatus();
public:
    
    ChatServer(io_service& ioService,
               const tcp::endpoint& participantsEndpoint,
               const tcp::endpoint& counterpartsEndpoint,
               const vector<tcp::resolver::iterator>& knownCounterparts) : ioService(ioService),
                                                                      participantsAcceptor(ioService, participantsEndpoint),
                                                                      counterpartsAcceptor(ioService, counterpartsEndpoint),
                                                                      messageSendTimer(ioService) {
        connectKnownCounterparts(knownCounterparts);
        startAcceptCounterparts();                                                                  
        startAcceptParticipants();
        checkEarliestMessageStatus();
                                                                          
    }
    
    void removeParticipant(shared_ptr<Participant> toRemove) {
        participants.erase(toRemove);
    }
    
    void removeCounterpart(shared_ptr<Counterpart> toRemove) {
        counterparts.erase(toRemove);
    }
    
    void putToLocalQueue(ChatMessage message) {
        localMessageQueue.push(message);
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
    
    bool haveMessageEarlier(ptime messageTime) {
        return localMessageQueue.front().getTime() < messageTime;
    }
    
    
    void updateEarliestMesageResults(shared_ptr<Counterpart> counterpart, bool result) {
        lastMessageEarliest.insert(make_pair(counterpart, result));
        if (getRespondedCounterparts() == counterparts) {
            bool messageEarliest = true;
            for (map<shared_ptr<Counterpart>, bool>::const_iterator i = lastMessageEarliest.begin(); i != lastMessageEarliest.end(); ++i) {
                if (! i->second) {
                    messageEarliest = false;
                }
            }
            lastMessageEarliest.clear();
            if (messageEarliest) {
                deliverEarliestMessage();
            } 
            //It's can be point to call checkEarliestMessageStatus via timer, but it's unconvinient
            //because this method wouldn't be called if localQueue is empty.
            //Now this case is handled.
            messageSendTimer.expires_from_now(milliseconds(100));
            messageSendTimer.async_wait(bind(
                                             &ChatServer::checkEarliestMessageStatus,
                                             this));
        }
    }
    
};
    
}

#include "Participant.hpp"
#include "Counterpart.hpp"


namespace ChatServerNamespace {

void ChatServer::startAcceptParticipants() {
    shared_ptr<Participant> newParticipant(new Participant(ioService, *this));
    participantsAcceptor.async_accept(newParticipant->getSocket(), bind(
                                                                     &ChatServer::acceptParticipantHandler,
                                                                     this,
                                                                     newParticipant,
                                                                     boost::asio::placeholders::error));
}

void ChatServer::acceptParticipantHandler(shared_ptr<Participant> participant, const error_code& error) {
    if (!error) {
        participants.insert(participant);
        participant->start();
    }
    startAcceptParticipants();
}
    
void ChatServer::startAcceptCounterparts() {
    shared_ptr<Counterpart> newCounterpart(new Counterpart(ioService, *this));
    counterpartsAcceptor.async_accept(newCounterpart->getSocket(), bind(
                                                                        &ChatServer::acceptCounterpartHandler,
                                                                        this,
                                                                        newCounterpart,
                                                                        boost::asio::placeholders::error));
}
    
void ChatServer::acceptCounterpartHandler(shared_ptr<Counterpart> counterpart, const error_code& error) {
    if (!error) {
        counterparts.insert(counterpart);
        counterpart->start();
    }
    startAcceptCounterparts();
}
    
void ChatServer::connectKnownCounterparts(const vector<tcp::resolver::iterator>& knownCounterparts) {
    for (vector<tcp::resolver::iterator>::const_iterator i = knownCounterparts.begin(); i != knownCounterparts.end(); ++i) {
        shared_ptr<Counterpart> newCounterpart(new Counterpart(ioService, *this));
        async_connect(newCounterpart->getSocket(), *i,
                      bind(&ChatServer::acceptCounterpartHandler,
                           this,
                           newCounterpart,
                           boost::asio::placeholders::error));
    }
}
    
void ChatServer::deliverEarliestMessage() {
    //IMPORTANT
    //This method MUST NOT be called if some counterpart has earlier messages.
    if (!localMessageQueue.empty()) {
        ChatMessage earliestMessage = localMessageQueue.front();
        localMessageQueue.pop();
        globalMessageQueue.push_back(earliestMessage);
        for (set<shared_ptr<Counterpart> >::iterator i = counterparts.begin(); i != counterparts.end(); ++i) {
            (*i)->deliverMessage(earliestMessage);  //Странное дело. Вроде оператор -> прожимает кучу разыменовываний разом. А тут не вышло (((
                                        //Видимо работает только для указателей...
        }
    }
}
    
    
void ChatServer::checkEarliestMessageStatus() {
    //This method starts procedure to check if earliest message on this host is earliest globally.
    if (!localMessageQueue.empty() && !counterparts.empty()) {
        ptime earliestMessageTime = localMessageQueue.front().getTime();
        for (set<shared_ptr<Counterpart> >::const_iterator i = counterparts.begin(); i != counterparts.end(); ++i) {
            (*i)->tryEarliest(earliestMessageTime);
        }
    } else {
        if (!localMessageQueue.empty()) {
            deliverEarliestMessage();
        }
        messageSendTimer.expires_from_now(milliseconds(100));
        messageSendTimer.async_wait(bind(
                                         &ChatServer::checkEarliestMessageStatus,
                                         this));
    }
}


    
}


using ChatServerNamespace::ChatServer;


#endif
