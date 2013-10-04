/*********************************************************************
 *
 *   Microchip USB HID Bootloader for PIC18F and PIC18LF versions of:
 *	 PIC18F4553/4458/2553/2458
 *	 PIC18F4550/4455/2550/2455
 *	 PIC18F4450/2450
 *   PIC18F14K50/13K50
 *
 *********************************************************************
 * FileName:        BootPIC18NonJ.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 3.32+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * File version         Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0					06/19/08	Original Version.  Adapted from 
 *									MCHPFSUSB v2.1 HID Bootloader
 *									for PIC18F87J50 Family devices.
 * 2.9f                 06/27/12    Added PIC18F45K50 Family devices
 *                                  and some robustness enhancements.
 *********************************************************************/

#include <p18cxxx.h>
#include "typedefs.h"
#include "usb.h"
#include "io_cfg.h"             // I/O pin mapping
#include "bootloader_protocol.h"

#pragma rom
const byte dfu_magic_code[4] = {0x35, 0xfa, 0x00, 0x18};

#define COMMIT_KEY          0xB5

// Variables
#pragma udata
unsigned char setup_done;
rx_packet_t rx_packet;
response_packet_t response_packet;

// Prototypes
void bootloader_init(void);
void bootloader_process_io(void);
void copy_to_table(ram byte* src);
void commit_write(unsigned char key);
void finalize_programming(void);
void ResetDeviceCleanly(void);

//void ClearWatchdog(void);
//void TableReadPostIncrement(void);

#pragma code

// Initialize the bootloader code
void bootloader_init(void)
{
    setup_done = FALSE;
}


// Main event loop of the bootloader
void bootloader_process_io(void)
{
        byte tblow;
        byte tbhigh;
        byte i;
        ROM byte* rp;
        unsigned int checksum;

	if(!mHIDRxIsBusy())	//Did we receive a command?
	{
            HIDRxReport((byte *)&rx_packet, 64);
			

            // The first byte of the received packet includes the command.
            switch(rx_packet.generic_packet.command) {
                case PROG_CMD_SETUP:
                    // Check if the address is in the allowable
                    // range
                    if ((rx_packet.setup_packet.address._word < LOW_PROG_ADDRESS) ||
                            (rx_packet.setup_packet.address._word > HIGH_PROG_ADDRESS))
                        // TODO: Return NAK
                        break;

                    // Set the table pointer
                    TBLPTRU = 0;
                    TBLPTRH = MSB(rx_packet.setup_packet.address);
                    TBLPTRL = LSB(rx_packet.setup_packet.address) & 0xe0;

                    // Copy the data in the packet into the
                    // table latches
                    copy_to_table(&(rx_packet.setup_packet.data_low));

                    // Set the setup flag to TRUE
                    setup_done = TRUE;
                    break;

                case PROG_CMD_EXEC:
                    // Make sure a SETUP packet has already been
                    // sent and processed
                    if (!setup_done)
                        // TODO: Return NAK
                        break;

                    // Copy the highest 32 bytes to the table latches
                    tblow = TBLPTRL;
                    tbhigh = TBLPTRH;
                    copy_to_table(&(rx_packet.exec_packet.data_high));
                    
                    // Restore the table pointer, so it will point to
                    // the actual page being touched
                    TBLPTRH = tbhigh;
                    TBLPTRL = tblow;

                    // Erase the flash page
                    EECON1 = 0b10010100;
                    commit_write(COMMIT_KEY);

                    // Write the data
                    EECON1 = 0b10000100;
                    commit_write(COMMIT_KEY);

                    // Clear the setup flag
                    setup_done = FALSE;
                    break;

                case PROG_CMD_FINALIZE:
                    // Calculate 16-bit checksum
                    checksum = 0;
                    for(rp = (rom byte*)LOW_PROG_ADDRESS; rp <= (rom byte*)HIGH_PROG_ADDRESS; rp++)
                        checksum += (unsigned char)(*rp);

                    // Verify it
                    if (rx_packet.finalize_packet.checksum._word != checksum) {
                        // Checksum failed - Transmit a Fail packet
                        while(mHIDTxIsBusy());
                        response_packet.result = 0x0;
                        response_packet.padding[0] = checksum & 0xff;
                        response_packet.padding[1] = (checksum >> 8) & 0xff;
			HIDTxReport((char *)&response_packet, 64);
                    }
                    else {
                        // Checksum passed - write magic code to EEPROM
                        // and transmit 'success' packet
                        while(mHIDTxIsBusy());
                        response_packet.result = 0x1;
			HIDTxReport((char *)&response_packet, 64);

                        // Write the magic code to the EEPROM
                        EECON1 = 0b00000100;
                        for(i = 0; i < 4; i++) {
                            EEADR = i;
                            EEDATA = dfu_magic_code[i];
                            EECON1bits.WREN = 1;
                            commit_write(COMMIT_KEY);
                        }
                    }
                    break;

                case PROG_CMD_RESET:
                    // Check that the unlock code
                    if ((rx_packet.reset_packet.unlock[0] != RESET_UNLOCK_0) ||
                        (rx_packet.reset_packet.unlock[1] != RESET_UNLOCK_1) ||
                        (rx_packet.reset_packet.unlock[2] != RESET_UNLOCK_2) ||
                        (rx_packet.reset_packet.unlock[3] != RESET_UNLOCK_3))
                        //TODO: Send Nak packet
                        break;
                    ResetDeviceCleanly();

                default:
                    break;

            }

        }
}//End ProcessIO()



void ResetDeviceCleanly(void)
{
    static unsigned char i;
    
	UCONbits.SUSPND = 0;		//Disable USB module
	UCON = 0x00;				//Disable USB module
	//And wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state. 
	//Otherwise host might not realize we disconnected/reconnected when we do the reset.
	//A basic for() loop decrementing a 16 bit number would be simpler, but seems to take more code space for
	//a given delay.  So do this instead:
	for(i = 0; i < 0xFF; i++)
	{
		WREG = 0xFF;
		while(WREG)
		{
			WREG--;
			_asm
			bra	0	//Equivalent to bra $+2, which takes half as much code as 2 nop instructions
			bra	0	//Equivalent to bra $+2, which takes half as much code as 2 nop instructions
			_endasm	
		}
	}
	Reset();    
	Nop();
	Nop();
}    

// Copies a block of data from the RAM to the table
// latches. This function always copies 32 bytes.
void copy_to_table(ram byte* src)
{
    byte i;
    for(i=0; i < 32; i++) {
        TABLAT = *src++;
        _asm tblwtpostinc _endasm
    }
}




//It is preferrable to only place this sequence in only one place in the flash memory.
//This reduces the probabilty of the code getting executed inadvertently by
//errant code.
void commit_write(unsigned char key)
{
	INTCONbits.GIE = 0;		//Make certain interrupts disabled for unlock process.

    //Check to make sure the caller really was trying to call this function.
    //If they were, they should always pass us the CORRECT_UNLOCK_KEY.
    if(key != COMMIT_KEY)
    {
        //Warning!  Errant code execution detected.  Somehow this 
        //UnlockAndActivate() function got called by someone that wasn't trying
        //to actually perform an NVM erase or write.  This could happen due to
        //microcontroller overclocking (or undervolting for an otherwise allowed
        //CPU frequency), or due to buggy code (ex: incorrect use of function 
        //pointers, etc.).  In either case, we should execute some fail safe 
        //code here to prevent corruption of the NVM contents.
        OSCCON = 0x03;  //Switch to INTOSC at low frequency
        while(1)
        {
            Sleep();
        }    
        Reset();
    }    
    
	_asm
	//Now unlock sequence to set WR (make sure interrupts are disabled before executing this)
	MOVLW 0x55
	MOVWF EECON2, 0
	MOVLW 0xAA
	MOVWF EECON2, 0
	BSF EECON1, 1, 0		//Performs write
	_endasm	
	while(EECON1bits.WR);	//Wait until complete (relevant when programming EEPROM, not important when programming flash since processor stalls during flash program)	
	EECON1bits.WREN = 0;  	//Good practice now to clear the WREN bit, as further protection against any accidental activation of self write/erase operations.
}	


//Note: The ClrWdt() and "_asm tblrdpostinc _endasm" are inline assembly language
//instructions.  The ClearWatchdog() and  TableReadPostIncrement() functions are 
//theoretically extraneous, since the operations being accomplished could be
//done without calling them as separate functions.  However, when using inline
//assembly language, the C compiler normally doesn't know what the code will
//actually do (ex: will it modify STATUS reg, WREG, BSR contents??).  As a 
//result, it is potentially dangerous for the C compiler to make assumptions,
//that might turn out not to be correct.  Therefore, the C18 compiler disables
//the compiler optimizations for a function, when one or more inline asm 
//instructions are located within the C function.  Therefore, to promote best
//code size optimizations from the C compiler, it is best to locate inline
//assembly sequences in their own separate C functions, that do not contain much
//other code (which could otherwise be optimized by the C compiler).  This often
//results in the smallest code size, and is the reason it is being done here.
void ClearWatchdog(void)
{
    ClrWdt();
}    
