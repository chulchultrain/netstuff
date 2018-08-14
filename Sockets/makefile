HOME_DIR=$(shell pwd)
SER_DIR=$(HOME_DIR)/Server

CC = g++
FLAGS = -I$(HOME_DIR) -c -Wall
CCMD = $(CC) $(FLAGS)

MCServer: $(SER_DIR)/MultiConnServer.o $(SER_DIR)/MultiConnServerMain.o
	$(CC) -o MCServer $(SER_DIR)/MultiConnServer.o $(SER_DIR)/MultiConnServerMain.o


$(SER_DIR)/MultiConnServer.o: $(SER_DIR)/MultiConnServer.h $(SER_DIR)/MultiConnServer.cpp
	$(CCMD) -o $(SER_DIR)/MultiConnServer.o $(SER_DIR)/MultiConnServer.cpp 

$(SER_DIR)/MultiConnServerMain.o: $(SER_DIR)/MultiConnServer.h $(SER_DIR)/MultiConnServerMain.cpp
	$(CCMD) -o $(SER_DIR)/MultiConnServerMain.o $(SER_DIR)/MultiConnServerMain.cpp 

