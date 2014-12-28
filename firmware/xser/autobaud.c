#include <GenericTypeDefs.h>
#include "HardwareProfile.h"
#include "Compiler.h"
#include "autobaud.h"

#define PULSE_TABLE_LENGTH 32

UINT16 AB_ShortPulseWidth;

// The pulse table is located at GPR7
#pragma udata PULSE_TABLE
UINT16 AB_PulseTable[32];
UINT8 AB_PulseTableIndex;

void AUTOBAUD_Init()
{
    // Enable interrupt-on-change and set it to high priority
    INTCON2bits.IOCIP = 1;
    INTCONbits.IOCIE = 1;

    // Configure timer 1 to fosc/4 clock source, 1:1 prescalar,
    // 16 bit R/W operation.
    T1CON = 0b00000011;
    T1GCON = 0;

    // Clear the pulse table
    for(AB_PulseTableIndex=0; AB_PulseTableIndex < PULSE_TABLE_LENGTH;
            AB_PulseTableIndex++)
    {
        AB_PulseTable[AB_PulseTableIndex] = AB_PulseTableIndex;
    }
    AB_PulseTableIndex = 31;

    AUTOBAUD_Abort();
}

void AUTOBAUD_Engage()
{
     AB_PulseTableIndex = 31;

    // Disbale the serial port
    RCSTA1bits.SPEN = 0;

    // Enable the Interrupt-on-change on the RX pin
    IOCCbits.IOCC7 = 1;
}

void AUTOBAUD_Abort()
{
    // Disable the Interrupt-on-change on the RX pin
//    IOCCbits.IOCC7 = 0;

    // Enable the serial port
//    RCSTA1bits.SREN = 1;

    AB_ShortPulseWidth = 0xffff;
}

// AUTOBAUD Data Acquisition Interrupt
//
// This interrupt handler is called upon pin change. In the highest supported
// line rate (150K) it will be executed 300K times per second, therefore it is
// hand optimized and kept to bare minimum.
// The handler uses Timer 1, which is clocked at fosc/4 (8MHz) to capture pin
// changes. The data is written to a table in GPR bank 7. After the table has
// been filled, the interrupt is disabled. The data processing routine will
// re-enable it after it has done processing it.
#pragma interrupt AUTOBAUD_Interrupt_Handler 
void AUTOBAUD_Interrupt_Handler()
{
    _asm
            // Read PORTC to clear the mismatch
            MOVF PORTC, 0, ACCESS

            // Clear the interrupt flag
            BCF INTCON, 0, ACCESS
          
            // Set the BSR to 7, to access the pulse table
            MOVLB 7

            // Set the indirect pointer to bank 7, data pointed
            // by AB_PulseTableIndex
            MOVLW 7
            MOVWF FSR2H, ACCESS
            MOVFF AB_PulseTable, FSR2L
            RLNCF AB_PulseTableIndex, 0, BANKED
            ADDWF FSR2L, 1, ACCESS

            // Copy the TMR1
            MOVFF TMR1L, POSTINC2
            MOVFF TMR1H, INDF2

            // Clear TMR1
            CLRF TMR1H, ACCESS
            CLRF TMR1L, ACCESS

            // Increment the table index
            DCFSNZ AB_PulseTableIndex, 1, BANKED
            BCF IOCC, 7, ACCESS

    _endasm


}

