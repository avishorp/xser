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
unsigned char AUTOBAUD_Service();

// Interrupt handler (for pin change interrupt)
void AUTOBAUD_Interrupt_Handler();

// Interrupt handler (for Timer 1 overflow interrupt)
void AUTOBAUD_Timer1_Interrupt_Handler();


#endif	/* AUTOBAUD_H */

