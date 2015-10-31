#include "transFunc.h"

using namespace std;

#define MAXTRANSBUFF 10
#define TIMEOUT 1000 // in ms

struct sockaddr_in receiver_addr;
int sockfd, slen = sizeof(receiver_addr);
static Byte lastReceivedChar;

Byte buf[MAXTRANSBUFF];
bool ack[MAXTRANSBUFF];
clock_t startTime[MAXTRANSBUFF];
TransWindow window = {0,0,0, MAXTRANSBUFF, buf, ack, startTime};

void* receiveResponse(void*);
void* receiveMessage(void*);

int main(int argc, char* argv[]){

// =========== Initializing socket ==================
	if(argc < 4){
		cout<<"Wrong number of arguments, should receive 3 arguments\n"<<endl;
		cout<<"./transmitter [ipAddress] [portNo] [TextFile]"<<endl;
		exit(EXIT_FAILURE);
	}

	char* ip_Addr = argv[1];
	int portNo = atoi(argv[2]);
	char* fileText = argv[3];

	//Opening a socket
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0){
		cout<<"Error creating socket"<<endl;
		exit(EXIT_FAILURE);
	}
	cout<<"Socket created to "<<ip_Addr<<":"<<portNo<<endl;

	//Initializing receiver address
	receiver_addr.sin_addr.s_addr = inet_addr(ip_Addr);
	receiver_addr.sin_family = AF_INET;
	receiver_addr.sin_port = htons(portNo);

// ================ Socket init finished ===================

	// Create thread for receiving response
	pthread_t recvResponse_thread;
	if(pthread_create(&recvResponse_thread, NULL, &receiveResponse, NULL) < 0){
		cout<<"Error creating thread"<<endl;
		exit(EXIT_FAILURE);
	}

	//Main thread
	//Keep sending bytes until EOF while not XOFF
	FILE *fp;
	fp = fopen(fileText, "r");

	if(fp == NULL){
		cout<<"Text File not found, terminating..."<<endl;
		exit(EXIT_FAILURE);
	}

	Byte ch;
	int msgno = 0;
	bool endfile = false;
	while(true){
		// Read message and put to window
		while(window.count <= window.maxsize / 2 && !endfile){
			if(lastReceivedChar != XOFF){
				if(fscanf(fp, "%c", &ch) == EOF){
					ch = Endfile; //Endfile sequence
					endfile = true;
				}
				addToBack(ch, &window);
			}
		}

		// Iterate through window and send frame which have not been ACK
		for(int i = window.front; i!=window.rear; i = (i+1) % window.maxsize){
			if(!window.ack[i]){
				double timeDif = (double)(clock() - window.startTime[i])/CLOCKS_PER_SEC * 1000;
				if(window.startTime[i]== -1 || timeDif>TIMEOUT){
					if(window.startTime[i] != -1)
						printf("Time out frame no %d\n",i);

					window.startTime[i] = clock();
					sendMessage(i, window.data[i], sockfd, receiver_addr, slen);
					printf("Sent msgno-%d: '%c'\n", i, window.data[i]);
				}
			}
			else if(i == window.front && window.ack[i]){
				//Slide
				removeFront(&window);
			}
		}

		if(endfile && window.front == window.rear)
			break; // All file sent
	}

	fclose(fp);

	//Creating a new thread, main thread will send data from text file
	//Child thread will receive XON/XOFF signal from receiver
	// pthread_t child_thread;
	// if(pthread_create(&child_thread, NULL, &receiveMessage, NULL) < 0){
	// 	cout<<"Error creating thread"<<endl;
	// 	exit(EXIT_FAILURE);
	// }


	return 0;	
}

void* receiveResponse(void*){
	// Haven't implemented checksum for receiving response

	RESP response;
	while(true){
		if(recvfrom(sockfd, &response, sizeof(RESP), 0, 
				(struct sockaddr*)&receiver_addr, (socklen_t*) &slen) == -1)
			{
				cout<<"Error receiving byte"<<endl;
				exit(EXIT_FAILURE);
			}

		if(response.ack == ACK){
			printf("ACK received for message no: %d\n", response.msgno);
			window.ack[response.msgno] = true;
		}
		else{
			printf("NAK received for message no: %d\n", response.msgno);
			
			// Resendings
			int msgno = response.msgno;
			int data = window.data[msgno];

			window.startTime[msgno] = clock();
			sendMessage(msgno, data, sockfd, receiver_addr, slen);
			printf("Sent msgno-%d: '%c'\n", msgno, data);
		}
	}
}

void* receiveMessage(void*){
	//Keep reading data from receiver until
	//Connection is terminated

	Byte ch;
	while(true){
		if(recvfrom(sockfd, &ch, sizeof(Byte), 0, 
			(struct sockaddr*)&receiver_addr, (socklen_t*) &slen) == -1)
		{
			cout<<"Error receiving byte"<<endl;
			exit(EXIT_FAILURE);
		}
		if(ch == XON)
			cout<<"XON received"<<endl;
		else if(ch == XOFF)
			cout<<"XOFF received"<<endl;

		lastReceivedChar = ch;
	}
}