/* 
 * File:   usart.h
 * Author: avishay
 *
 * Created on 20 ??? 2013, 09:44
 */

#ifndef USART_H
#define	USART_H

#include "HardwareProfile.h"
#include "GenericTypeDefs.h"

// Function prototypes
void USART_Init();
void USART_SetBaud(unsigned long baud);
void USART_SetBRG(UINT16 brg);
void USART_SetEncoding(unsigned char enc);
void USART_LB_SendByte(unsigned char b);
unsigned char USART_LB_GetByte();
BOOL USART_LB_IsRxAvail();

// Maximal supported baud rate
#define MAX_BAUD_RATE 230400

// Macros
#ifndef DBG_SERIAL_LOOPBACK

// Normal operation
#define USART_SendByte(b)   TXREG1 = b
#define USART_GetByte()     RCREG1
#define USART_IsTxEmpty() TXSTA1bits.TRMT
#define USART_IsRxAvail() PIR1bits.RCIF

#else
// Loopback
#define USART_SendByte(b) USART_LB_SendByte(b)
#define USART_GetByte()   USART_LB_GetByte()
#define USART_IsTxEmpty() USART_LB_IsTxEmpty()
#define USART_IsRxAvail() USART_LB_IsRxAvail()

#endif

#endif	/* USART_H */

