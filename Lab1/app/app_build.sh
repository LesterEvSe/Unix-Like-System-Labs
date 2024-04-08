#!/bin/bash

g++ -o server server.cpp -lpthread -lsqlite3
./server
