//
//  chat_server.cpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#include "ChatServer.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <vector>

using namespace ChatServerNamespace;

using boost::asio::io_service;
using boost::asio::ip::tcp;

using std::vector;

int main(int argc, char* argv[]) {
    try
    {
        if (argc < 3 || argc % 2 == 0)
        {
            std::cerr << "Usage: ChatServer <PortForParticipants> <PortForCounterpart> [<CounterpartHost> <CounterpartPort>]\n";
            return 1;
        }
        
        io_service ioService;
        tcp::endpoint participantsEndpoint(tcp::v4(), atoi(argv[1]));
        tcp::endpoint counterpartsEndpoint(tcp::v4(), atoi(argv[2]));
        
        vector<tcp::resolver::iterator> counterparts;
        tcp::resolver resolver(ioService);
        for (int i = 3; i < argc; i += 2) {
            tcp::resolver::query query(argv[i], argv[i + 1]);
            counterparts.push_back(resolver.resolve(query));
        }
        
        ChatServer server(ioService, participantsEndpoint, counterpartsEndpoint, counterparts);
        ioService.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}