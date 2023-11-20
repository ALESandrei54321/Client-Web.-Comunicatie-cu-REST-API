CC = g++
CFLAGS = -Wall -Wextra -std=c++11
OBJ=client.o requests.o helpers.o buffer.o

all: client

client: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

run: client
	./client

clean:
	rm -f *.o client
