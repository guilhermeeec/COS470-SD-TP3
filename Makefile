CC = g++
CFLAGS = -Wall -std=c++11
RPCFLAGS = -lrpc -lpthread

PROCESS_SOURCES = process.cpp utils.cpp
COORDINATOR_SOURCES = coordinator.cpp utils.cpp process_info.cpp process_fifo.cpp

PROCESS_OBJECTS = $(PROCESS_SOURCES:.cpp=.o)
COORDINATOR_OBJECTS = $(COORDINATOR_SOURCES:.cpp=.o)

all: process coordinator

process: $(PROCESS_OBJECTS)
	$(CC) $(CFLAGS) $(PROCESS_OBJECTS) $(RPCFLAGS) -o process

coordinator: $(COORDINATOR_OBJECTS)
	$(CC) $(CFLAGS) $(COORDINATOR_OBJECTS) $(RPCFLAGS) -o coordinator

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean: clean_resources clean_results

clean_resources:
	rm -rf $(wildcard *.o) process coordinator

clean_results:
	rm -rf resultado.txt log.txt
