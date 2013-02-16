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

using namespace ChatServerNamespace;

using boost::asio::io_service;
using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try
    {
        if (argc < 3 && argc > 4)
        {
            std::cerr << "Usage: ChatServer <PortForParticipants> <PortForCounterpart> [<CounterpartHost> <CounterpartPort>]\n";
            return 1;
        }
        
        io_service ioService;
        tcp::endpoint participantsEndpoint(tcp::v4(), atoi(argv[1]));
        tcp::endpoint counterpartsEndpoint(tcp::v4(), atoi(argv[2]));
        ChatServer server(ioService, participantsEndpoint, counterpartsEndpoint);
        
        ioService.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}