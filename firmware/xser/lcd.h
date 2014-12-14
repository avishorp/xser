/* 
 * File:   lcd.h
 * Author: avishay
 *
 * Description: LCD related functions
 */

#ifndef LCD_H
#define	LCD_H

// Millisecond counter - counts every 1mS
extern UINT16 Global_MS_Counter;

// Display types
#define DISP_TYPE_TEST   0   // Test pattern (all segments on)
#define DISP_TYPE_NUMBER 1   // Decimal number between 0 to 199
#define DISP_TYPE_WAIT   2   // Wait pattern (running segment)
#define DISP_TYPE_ACT    3   // Activity indicator (A + two segments)
#define DISP_TYPE_BAUD   4   // Auto baud indication (b + running segment)

// Initialize the LCD system
void LCD_Init();

// Set the type of the display to one of the
// types in the list
void LCD_SetDisplayType(unsigned char type);

// Sets the value associated with the display type.
// The argument is interpreded according to the current
// display type:
//  DISP_TYPE_NUMBER - The argument is the number to show
//  DISP_TYPE_ACT - Bits 0 and 1 of the argument designate
//                  TX and RX activity
//  DISP_TYPE_BAUD - Any non-zero value denotes activity
void LCD_SetDisplayValue(unsigned char value);

#endif	/* LCD_H */

