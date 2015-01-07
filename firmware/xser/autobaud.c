#include <GenericTypeDefs.h>
#include "HardwareProfile.h"
#include "Compiler.h"
#include "autobaud.h"

#define PULSE_TABLE_LENGTH 32
#define MIN_BAUD           4000  // Minimal supported baud rate
#define MAX_BAUD           150000// Maximal supported baud rate
#define TOLERANCE          10    // Frequency tolerance

#define TIMER1_CLOCK_FREQ  8000000
#define MIN_VALID_PULSE    (TIMER1_CLOCK_FREQ/MAX_BAUD)
#define MAX_VALID_PULSE    (TIMER1_CLOCK_FREQ/MIN_BAUD)*20
#define INFINITE           0xffff
#define BINS               20

BOOL AB_Engaged;
UINT16 AB_ShortPulseWidth;
UINT16 AB_ShortPulseMin;
UINT16 AB_ShortPulseMax;
UINT16 AB_Bin[BINS];
UINT16 AB_TotalPulses;
UINT16 AB_MatchingPulses;
UINT16 AB_UnmatchingPulses;

UINT8 AB_ProcessingIndex;

// The pulse table is located at GPR7
#pragma udata PULSE_TABLE
UINT16 AB_PulseTable[32];
UINT8 AB_PulseTableIndex;

void AUTOBAUD_ClearPulseTable();
void AUTOBAUD_ClearBins();
void AUTOBAUD_SetShortPulse(UINT16 p);


void AUTOBAUD_Init()
{
    AB_Engaged = FALSE;

    // Enable interrupt-on-change and set it to high priority
    INTCON2bits.IOCIP = 1;
    INTCONbits.IOCIE = 1;

    // Configure timer 1 to fosc/4 clock source, 1:1 prescalar,
    // 16 bit R/W operation.
    T1CON = 0b00000011;
    T1GCON = 0;

    AUTOBAUD_ClearPulseTable();

    AUTOBAUD_Abort();
}

void AUTOBAUD_Engage()
{
    // Prepare the processing engine
    AUTOBAUD_ClearBins();
    AB_TotalPulses = 0;
    AUTOBAUD_SetShortPulse(INFINITE);
    AB_ProcessingIndex = PULSE_TABLE_LENGTH - 2;

    // Disbale the serial port
    RCSTA1bits.SPEN = 0;

    // Enable the Interrupt-on-change on the RX pin
    IOCCbits.IOCC7 = 1;

    AB_Engaged = TRUE;
}

void AUTOBAUD_Abort()
{
    AB_Engaged = FALSE;
    // Disable the Interrupt-on-change on the RX pin
//    IOCCbits.IOCC7 = 0;

    // Enable the serial port
//    RCSTA1bits.SREN = 1;

    AUTOBAUD_SetShortPulse(INFINITE);
;
}

void AUTOBAUD_ClearPulseTable()
{
    for(AB_PulseTableIndex=0; AB_PulseTableIndex < PULSE_TABLE_LENGTH;
            AB_PulseTableIndex++)
    {
        AB_PulseTable[AB_PulseTableIndex] = 0;
    }
    AB_PulseTableIndex = PULSE_TABLE_LENGTH - 1;
    AB_ProcessingIndex = PULSE_TABLE_LENGTH - 1;
}

void AUTOBAUD_ClearBins()
{
    UINT8 k;
    for(k=0; k < BINS; k++)
        AB_Bin[k] = 0;

    AB_MatchingPulses = 0;
    AB_UnmatchingPulses = 0;
}

void AUTOBAUD_SetShortPulse(UINT16 p)
{
    UINT8 d;
    if (p > 200)
        d = 10;
    else
        d = 25;
    d = 30;

    if (p == INFINITE) {
        AB_ShortPulseWidth = INFINITE;
        AB_ShortPulseMin = INFINITE;
        AB_ShortPulseMax = INFINITE;
    }
    else {
        AB_ShortPulseWidth = p;
        AB_ShortPulseMin = AB_ShortPulseWidth - d;
        AB_ShortPulseMax = AB_ShortPulseWidth + d;
    }
}

void AUTOBAUD_Service()
{
    UINT16 p;
    UINT8 k;
    UINT16 min, max;

    if (!AB_Engaged)
        return;

    // Check if there's any data to process. The sample buffer is filled
    // by the interrupt routine from the highest index downto 0. The first
    // data item (index PULSE_TABLE_LENGTH-1) is garbage because it's the first
    // edge captured. The last data item (index 0) cannot be used because the
    // interrupt is disabled when the index is 0.
    if (AB_ProcessingIndex <= AB_PulseTableIndex)
        // No new data to process, abort
        return;

    // If the processing pointer has reached 0, re-kick the data
    // acquisition interrupt
    if (AB_PulseTableIndex == 0) {
        AUTOBAUD_ClearPulseTable();
        IOCCbits.IOCC7 = 1;
        return;
    }

    p = AB_PulseTable[AB_ProcessingIndex];
    AB_ProcessingIndex--;

    // First of all, filter out pulses that are two short (noise) or too
    // long (timeouts)
    if ((p < MIN_VALID_PULSE) || (p > MAX_VALID_PULSE))
        return;

    AB_TotalPulses++;

    // If the pulse is shorter than the shortest, set it as the new short pulse
    if (p < AB_ShortPulseMin) {
        AUTOBAUD_SetShortPulse(p);
        AUTOBAUD_ClearBins();
    }
    else {
        min = AB_ShortPulseMin;
        max = AB_ShortPulseMax;
        for(k = 0; k < BINS; k++) {
            if ((p >= min) && (p <= max)) {
                AB_Bin[k]++;
                AB_MatchingPulses++;
                return;
            }
            min += AB_ShortPulseMin;
            max += AB_ShortPulseMax;
        }

        AB_UnmatchingPulses++;
    }
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

            // Read PORTC to clear the mismatch
            MOVF PORTC, 0, ACCESS

            // Clear the interrupt flag
            BCF INTCON, 0, ACCESS

    _endasm


}

