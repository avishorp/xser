#include "Compiler.h"
#include "usart.h"

#ifdef DBG_SERIAL_LOOPBACK
unsigned char USART_LB_Char;
BOOL USART_LB_Avail;
#endif

void USART_Init()
{
    // Enable asynchronous transmission, 8-bit
    // high baud rate
    TXSTA1 = 0b00100100;

    // Enable serial port & receiver
    RCSTA1 = 0b10010000;

    // Enable 16 bit Baud Rate generator
    BAUDCON1 = 0b00001000;

#ifdef DBG_SERIAL_LOOPBACK
    USART_LB_Avail = FALSE;
#endif
}

void USART_SetBaud(unsigned long baud)
{
    // Calculate the divisor required for the
    // specified baud rate (page 282 in the datasheet)
    unsigned long div = (CLOCK_FREQ/4)/baud - 1;

    // Set the registers
    SPBRG = div & 0xff;
    SPBRGH = ((unsigned char)(div >> 8)) & 0xff;

}

void USART_SetEncoding(unsigned char enc)
{
}

void USART_LB_SendByte(unsigned char b)
{
#ifdef DBG_SERIAL_LOOPBACK
    USART_LB_Avail = TRUE;
    USART_LB_Char = b;

    TXREG1 = b;
#endif
}

unsigned char USART_LB_GetByte()
{
#ifdef DBG_SERIAL_LOOPBACK
    if (USART_LB_Avail) {
        USART_LB_Avail = 0;
        return USART_LB_Char;
    }
    else
        return 0;
#endif
}

BOOL USART_LB_IsRxAvail()
{
#ifdef DBG_SERIAL_LOOPBACK
    return USART_LB_Avail && (TXSTA1bits.TRMT==1);
#endif
}

BOOL USART_LB_IsTxEmpty()
{
#ifdef DBG_SERIAL_LOOPBACK
    return !USART_LB_IsRxAvail();
#endif
}


