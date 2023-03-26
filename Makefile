# This file is part of the "C++ Socket Programming" Assignment.

# Makefile for the "C++ Socket Programming".
# this file is used to compile the project.

.DEFAULT_GOAL := all

# compiler is set to g++
CC=g++

# compiler flags are set to -std=c++11
CFLAGS=-std=c++11

# the following are the targets for the makefile
# the targets are:
# server: compiles the server.cpp file
# -lpthread is used to link the pthread library to the server executable file
server:
	$(CC) $(CFLAGS) -lpthread -o server server.cpp

# client: compiles the client.cpp file
client:
	$(CC) $(CFLAGS) -o client client.cpp

# all: compiles both the server and client
all: server client

# clean: removes the server and client executables
clean:
	rm -f server client
