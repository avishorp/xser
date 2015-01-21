/* 
 * File:   autobaud.h
 * Author: avishay
 *
 * Created on 14 ????? 2014, 11:01
 */

#ifndef AUTOBAUD_H
#define	AUTOBAUD_H

////////////////////////////////////////
// Autobaud algorithm tuning parameters
////////////////////////////////////////

// The value below which pulses are ignored
#define AUTOBAUD_NOISE_FILTER    32

// The value above which pulses are ignored
#define AUTOBAUD_TIMEOUT_FILTER  6500

// Initialize the AutoBaud subsystem
void AUTOBAUD_Init();

// Start baud detection process
void AUTOBAUD_Engage();

// Abort baud detection process
void AUTOBAUD_Abort();

// Auto-baud main loop processing
void AUTOBAUD_Service();

// Interrupt handler (for pin change interrupt)
void AUTOBAUD_Interrupt_Handler();

// Interrupt handler (for Timer 1 overflow interrupt)
void AUTOBAUD_Timer1_Interrupt_Handler();

// The event result is accessed directly as a variable
// due to an apparent compiler bug causing the functions
// in this module not to return values correctly
extern unsigned char AB_Event;


#endif	/* AUTOBAUD_H */

