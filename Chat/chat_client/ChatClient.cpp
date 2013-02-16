//
//  chat_server.cpp
//  Chat
//
//  Created by Dmitriy on 2/11/13.
//  Copyright (c) 2013 Dmitriy. All rights reserved.
//

#include "ChatClient.hpp"
#include "ChatMessage.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <string>


using namespace ChatClientNamespace;

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::thread;
using boost::bind;

using std::string;
using std::getline;
using std::cin;

int main(int argc, char* argv[]) {
    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: ChatClient <myName> <host> <port>\n";
            return 1;
        }
        
        io_service ioService;
        
        tcp::resolver resolver(ioService);
        tcp::resolver::query query(argv[2], argv[3]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        
        string name = argv[1];
        ChatClient client(ioService, iterator, name);
        
        thread t(bind(&io_service::run, &ioService));
        
        string line;
        while (getline(cin, line))
        {
            if (line.size() > 0) {
                ChatMessage message(line);
                client.send(message);
            }
        }
        
        client.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}