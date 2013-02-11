CC = g++
CFLAGS = -W -Wall
LFLAGS = -L/opt/local/lib -lboost_system-mt -lboost_thread-mt
BOOST = -I/opt/local/include

SERVEROBJECTS = chat_server.o
CLIENTOBJECTS = chat_client.o
SERVER = chat_server
CLIENT = chat_client

ALL = chat_server chat_client

RM = rm -f

all: $(ALL)


$(SERVER): $(SERVEROBJECTS)
	$(CC) -o $@ $(SERVEROBJECTS) $(LFLAGS)
    
$(CLIENT): $(CLIENTOBJECTS)
	$(CC) -o $@ $(CLIENTOBJECTS) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(BOOST) -c $< 

chat_server.o: chat_server.hpp chat_session.hpp chat_room.hpp chat_message.hpp chat_server.cpp
chat_client.o: chat_client.hpp chat_message.hpp chat_client.cpp

clean:
	$(RM) $(SERVEROBJECTS) $(CLIENTOBJECTS) $(CLIENT) $(SERVER)
