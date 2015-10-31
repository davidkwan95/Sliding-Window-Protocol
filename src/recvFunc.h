/*
* File : recvFunc.h
*/

#ifndef recvFunc_h

#define recvFunc_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <bitset>
#include <cstring>
#include "dcomm.h"

#define MAXRECVBUFF 10 

using namespace std;

typedef struct RecvWindow
{
	//Front and rear of the window
	unsigned int front;
	unsigned int rear;
	unsigned int maxsize;
	Byte *data;
	bool *received; //Whether the frame has been received or not (include error checking)
} RecvWindow;

//Send Acknowledgement
void sendACK(Byte ack, int sockfd, struct sockaddr_in sender_addr, 
	int slen, unsigned int msgno, int checksum);

// CRC checksum
string MakeCRC(string BitString);
string getBitString(MESGB message);
bool isCheckSumCorrect(MESGB message);
bool isFrameValid(MESGB message);

void extendWindow(RecvWindow* window);
void shrinkWindow(RecvWindow* window);

void insertIntoProcessBuf(Byte data, QTYPE *queue, int sockfd, struct sockaddr_in sender_addr, int slen);

#endif