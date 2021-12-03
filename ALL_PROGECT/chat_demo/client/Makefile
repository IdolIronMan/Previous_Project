CC = g++
TARGET = client
DEPEND = main.o chat_message.o chat_client.o

$(TARGET): $(DEPEND)
	$(CC) $^ -o $@  -pthread

clean:
	rm -f $(TARGET)	
	rm -f $(DEPEND)