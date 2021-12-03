
CC = g++
TARGET = server
DEPEND = main.o chat_server.o tcp_acceptor.o  chat_message.o chat_session.o  chat_room.o

$(TARGET): $(DEPEND)
	$(CC) $^ -o $@  -pthread


clean:
	rm -f $(TARGET)
	rm -f $(DEPEND)




