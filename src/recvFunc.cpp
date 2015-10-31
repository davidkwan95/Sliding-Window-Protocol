#include "recvFunc.h"

using namespace std;

void sendACK(Byte ack, int sockfd, struct sockaddr_in sender_addr, int slen, 
	unsigned int msgno, int checksum)
{
	RESP response;
    response.ack = ack;
    response.msgno = msgno;
    response.checksum = checksum;

	if(sendto(sockfd, &response, sizeof(RESP), 0, 
     	(struct sockaddr *) &sender_addr, slen) == -1)
	{	
		cout<<"Error sending response"<<endl;
        exit(EXIT_FAILURE);    	
    }
}

//The CRC implementation code is using open source code with a little bit modification
// ==========================================================================
// CRC Generation Unit - Linear Feedback Shift Register implementation
// (c) Kay Gorontzi, GHSi.de, distributed under the terms of LGPL
// ==========================================================================
string MakeCRC(string BitString)
{
   //P = 11010101
   static char Res[8];                                 // CRC Result
   char CRC[7];
   int  i;
   char DoInvert;
   
   for (i=0; i<7; ++i)  CRC[i] = 0;                    // Init before calculation
   
   for (i=0; i<BitString.length(); ++i)
      {
      DoInvert = ('1'==BitString[i]) ^ CRC[6];         // XOR required?

      CRC[6] = CRC[5] ^ DoInvert;
      CRC[5] = CRC[4];
      CRC[4] = CRC[3] ^ DoInvert;
      CRC[3] = CRC[2];
      CRC[2] = CRC[1] ^ DoInvert;
      CRC[1] = CRC[0];
      CRC[0] = DoInvert;
      }
      
   for (i=0; i<7; ++i)  Res[6-i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
   Res[7] = 0;                                         // Set string terminator

   return(Res);
}
// =================== CRC CODE ENDS HERE =====================

//Generate BitString from message (not included checksum)
string getBitString(MESGB message){
    string BitString = "";
    BitString += bitset<8>(message.soh).to_string();
    BitString += bitset<8>(message.msgno).to_string();
    BitString += bitset<8>(message.stx).to_string();
    BitString += bitset<8>(message.data).to_string();
    BitString += bitset<8>(message.etx).to_string();
    BitString += bitset<7>(message.checksum).to_string();

    return BitString;
}

bool isCheckSumCorrect(MESGB message){
    string BitString = getBitString(message);
    string BitCheckSum = MakeCRC(BitString);

    return (bitset<7>(BitCheckSum).to_ulong()) == 0;
}

bool isFrameValid(MESGB message){
    return (message.soh == SOH && message.msgno < MAXRECVBUFF && 
            message.stx == STX && message.etx == ETX && isCheckSumCorrect(message));
}

void extendWindow(RecvWindow* window){
    window->received[window->rear] = false;
    window->rear = (window->rear + 1) % window->maxsize;
}

void shrinkWindow(RecvWindow* window){
    window->front = (window->front + 1) % window->maxsize;
}

// Insert data from window into process Buffer
void insertIntoProcessBuf(Byte data, QTYPE *queue, int sockfd, struct sockaddr_in sender_addr, int slen){
    queue->data[queue->rear++] = data;
    queue->rear %= queue->maxsize;
    queue->count++;

    // Sending XON and XOFF not implemented yet

    // if(queue->count > UPPERLIMIT && !send_xoff){
    //  cout<<"Buffer > minimum upperlimit. Sending XOFF"<<endl;
    //  Byte xoff;
    //  xoff = (Byte) XOFF;
    //  if(sendto(sockfd, &xoff, sizeof(Byte), 0, 
    //      (struct sockaddr *) &sender_addr, slen) == -1)
    //  {
    //      cout<<"Sending XOFF failed"<<endl;
    //  }
    //  send_xoff = true;
    // }
}