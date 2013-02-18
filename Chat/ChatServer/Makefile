CC = g++
CFLAGS = -W -Wall -g
LFLAGS = -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_date_time-mt
BOOST = -I/opt/local/include

SERVEROBJECTS = ChatServer.o
SERVER = ChatServer

ALL = $(SERVER)

RM = rm -f

all: $(ALL)


$(SERVER): $(SERVEROBJECTS)
	$(CC) -o $@ $(SERVEROBJECTS) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(BOOST) -c $< 

ChatServer.o: ChatServer.cpp ChatServer.hpp CounterpartMessage.hpp ParticipantResponse.hpp Counterpart.hpp Participant.hpp Message.hpp ChatMessage.hpp

clean:
	$(RM) $(SERVEROBJECTS) $(CLIENTOBJECTS) $(CLIENT) $(SERVER)
