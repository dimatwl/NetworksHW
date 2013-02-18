CC = g++
CFLAGS = -W -Wall -g
LFLAGS = -L/opt/local/lib -lboost_system-mt -lboost_thread-mt -lboost_date_time-mt
BOOST = -I/opt/local/include

CLIENTOBJECTS = ChatClient.o
CLIENT = ChatClient

ALL = $(CLIENT)

RM = rm -f

all: $(ALL)


$(CLIENT): $(CLIENTOBJECTS)
	$(CC) -o $@ $(CLIENTOBJECTS) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(BOOST) -c $< 

ChatClient.o: ChatClient.cpp ChatClient.hpp ParticipantRequest.hpp Message.hpp ChatMessage.hpp

clean:
	$(RM) $(CLIENTOBJECTS) $(CLIENT)
