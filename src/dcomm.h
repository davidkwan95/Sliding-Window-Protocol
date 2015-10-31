/*
* File : dcomm.h
*/

#ifndef _DCOMM_H_

#define _DCOMM_H_
/* ASCII Const */
#define SOH 1
#define STX 2
#define ETX 3
#define ENQ 5
#define ACK 6
#define BEL 7
#define CR 13
#define LF 10
#define NAK 21
#define Endfile 26
#define ESC 27


/* XON/XOFF protocol */
#define XON (0x11)
#define XOFF (0x13)
/* Const */
#define BYTESIZE 256 /* The maximum value of a byte */
#define MAXLEN 1024 /* Maximum messages length */

typedef unsigned char Byte;

typedef struct QTYPE
{
	unsigned int count;
	unsigned int front;
	unsigned int rear;
	unsigned int maxsize;
	Byte *data;
} QTYPE;

typedef struct MESGB
{
	Byte soh;
	unsigned int msgno;
	Byte stx;
	Byte data; //For this assignment, length=1
	Byte etx;
	Byte checksum; //Still doubting
} MESGB;

typedef struct RESP{
	Byte ack;
	unsigned int msgno;
	Byte checksum;
} RESP;

#endif