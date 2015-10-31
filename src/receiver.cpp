/*
* File : T1_rx.cpp
*/

#include "recvFunc.h"

using namespace std;

/* Delay to adjust speed of consuming buffer, in seconds */
#define DELAY 500
/* Define receive buffer size */
#define RXQSIZE 8
/* Define minimum upperlimit */
#define UPPERLIMIT 4
/* Define maximum lowerlimit */
#define LOWERLIMIT 2

Byte rxbuf[RXQSIZE];
QTYPE rcvq = { 0, 0, 0, RXQSIZE, rxbuf };
QTYPE *rxq = &rcvq;
bool send_xoff = false;

/* Socket */
int sockfd; // listen on sock_fd
int byteCounter = 0; //Number of bytes received
int byteConsumed = 0;
struct sockaddr_in receiver_addr, sender_addr;
int slen = sizeof(sender_addr);
int portNo;

// Window
Byte buf[MAXRECVBUFF];
bool received[MAXRECVBUFF];
RecvWindow window = {0, MAXRECVBUFF/2 - 1, MAXRECVBUFF, buf, received};

/* Functions declaration */
static Byte rcvchar(int sockfd, QTYPE *queue);
static Byte q_get(QTYPE *);

void* consumeBuffer(void*);

int main(int argc, char *argv[]) {
// ============ Initializing Socket =====================
	if(argc < 2){
		cout<<"Wrong number of arguments, should receive 1 argument"<<endl;
		cout<<"./receiver [portNo]"<<endl;
		exit(EXIT_FAILURE);
	}

	Byte c;
	portNo = atoi(argv[1]);

	//Opening a socket
	sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sockfd < 0){
		cout<<"Error creating socket"<<endl;
		exit(EXIT_FAILURE);
	}
	cout<<"Socket created ..."<<endl;

	//Initializing receiver address (localhost)
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	receiver_addr.sin_port = htons(portNo);

	//Bind address to the socket
	if(bind(sockfd, (struct sockaddr *) &receiver_addr, sizeof(receiver_addr)) < 0){
		cout<<"Error binding"<<endl;
		exit(EXIT_FAILURE);
	}
	cout<<"Binding at port:"<<portNo<<endl;

// ============ Socket init finished ================

	/* Create child thread, for consuming buffer */
	pthread_t child_thread;
	if(pthread_create(&child_thread, NULL, &consumeBuffer, NULL) < 0){
		cout<<"Error creating thread"<<endl;
		exit(EXIT_FAILURE);
	}

	// Main thread for receiving message using
	// sliding window protocol
	MESGB message;
    while(true){
	    if(recvfrom(sockfd, &message, sizeof(MESGB), 0, 
				(struct sockaddr *) &sender_addr, (socklen_t *) &slen) == -1)
		{     
	        cout<<"Error receiving byte"<<endl;
	        exit(EXIT_FAILURE);
	    }

    	// Send response back
	    if(isFrameValid(message) && rcvq.count < RXQSIZE){
	    	int msgno = message.msgno;
	    	Byte data = message.data;
	    	window.data[msgno] = data;

	    	sendACK(ACK, sockfd, sender_addr, slen, msgno, message.checksum);
	    	if(data != Endfile){
		    	printf("Frame no: %d received (Byte received: %c)\n", msgno, data);
		    	window.received[msgno] = true;
	    	}
		    else
		    	break;
	    }
	    else if(rcvq.count >= RXQSIZE){
	    	cout<<"Fail: Buffer Full, not sending ACK"<<endl;
	    }
	    else{
	    	cout<<"Fail: Wrong checksum. Sending NAK"<<endl;
	    	sendACK(NAK, sockfd, sender_addr, slen, message.msgno, message.checksum);
	    }

	    for(int i = window.front; i != window.rear; i = (i+1)%window.maxsize){
	    	if(i == window.front && window.received[i]){
	    		insertIntoProcessBuf(window.data[i], rxq, sockfd, sender_addr, slen);
	    		shrinkWindow(&window);
	    		extendWindow(&window);
	    	}
	    }
	}
	while(rxq->count!=0){
		//Do Nothing, waiting buffer being consumed
	}
	
	return 0;
}

// q_get returns a pointer to the buffer where data is read 
// or NULL if buffer is empty.

static Byte q_get(QTYPE *queue) {
	Byte *current;
	/* Nothing in the queue */
	if (!queue->count) return '\0';
	
	/*
	Retrieve data from buffer, save it to "current" and "data"
	If the number of characters in the receive buffer is below certain
	level, then send XON.
	Increment front index and check for wraparound.
	*/
	Byte c = queue->data[queue->front++];
	queue->front %= queue->maxsize;
	queue->count--;

	if(c != Endfile){
		printf("Consumed byte %d: '%c'\n",++byteConsumed, c);

	// XON and XOFF not yet implemented

	// if(queue->count == LOWERLIMIT && send_xoff){
	 //    	cout<<"Buffer < maximum lowerlimit. Sending XON"<<endl;
	 //    	Byte xon;
	 //    	xon = (Byte) XON;
	 //    	if(sendto(sockfd, &xon, sizeof(Byte), 0, 
	 //    		(struct sockaddr *) &sender_addr, slen) == -1)
	 //    	{
	 //    		cout<<"Sending XON failed"<<endl;
	 //    	}
	 //    	send_xoff = false;
		// }
	}

	return c;
}

void* consumeBuffer(void*){
	//Consume Buffer with delay
	while(true){
		q_get(rxq);
		usleep(DELAY * 1000); //usleep parameter is useconds
	}
}