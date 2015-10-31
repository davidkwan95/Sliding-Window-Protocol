/*
* File : transFunc.h
*/

#ifndef transFunc_h

#define transFunc_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>
#include <cstring>
#include <bitset>
#include <time.h>
#include "dcomm.h"

using namespace std;

typedef struct TransWindow
{
	//Front and rear of the window
	unsigned int count;
	unsigned int front;
	unsigned int rear;
	unsigned int maxsize;
	Byte *data;
	bool *ack; //Whether the frame sent has been acknowledge or not
	clock_t *startTime;
} TransWindow;

//Send message
void sendMessage(Byte msgno, Byte data, int sockfd, struct sockaddr_in receiver_addr, int slen);

//Get string of CRC from bitstring
string MakeCRC(string BitString);

//Generate BitString from message
string getBitString(MESGB message);

//Generate checksum from message to be sent
Byte getCheckSum(MESGB message);

//Add data to back of window
void addToBack(Byte data, TransWindow *window);

//Remove front data from window
void removeFront(TransWindow *window);

#endif
