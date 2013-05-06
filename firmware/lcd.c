//

#include "compiler.h"
#include "lcd.h"
#include "HardwareProfile.h"

unsigned char LCD_DisplayType;
unsigned short LCD_AnimDelay;
unsigned char LCD_State;

#define ANIM_DELAY_VALUE 1000

// Forwards
void LCD_SetDigit12(unsigned char segments);
void LCD_SetDigit3(unsigned char segments);


#define LCD_CriticalSectionBegin() PIE1bits.TMR2IE = 0;
#define LCD_CriticalSectionEnd() PIE1bits.TMR2IE = 1;


#pragma interrupt LCD_Interrupt_Handler
void LCD_Interrupt_Handler()
{
    // Clear Timer 2 interrupt flag
    PIR1bits.TMR2IF = 0;

    // Invert all LCD pins
    LCD_Invert_Bits();

    // Animation Delay
    LCD_AnimDelay -= 1;
    if (LCD_AnimDelay == 0) {
        LCD_AnimDelay = ANIM_DELAY_VALUE;

        if (LCD_DisplayType == DISP_TYPE_WAIT) {
            //    a      a
            //   ---    ---
            // f|          | b
            // e|          | c
            //   ---    ---
            //    d      d
            //
            // DIGIT 2  DIGIT 3
            //

            // Set the LCD segments
            LCD_SetDigit3(LCD_State & 0x0f);
            LCD_SetDigit12(((LCD_State >> 1) & 0x38) + (LCD_State >> 7));

            // Calculate the new state
            LCD_State <<= 1;
            if (LCD_State == 0x00)
                LCD_State = 0x01;
        }
    }
}

#pragma code high_vector=0x08
void interrupt_at_low_vector(void)
{
  _asm GOTO LCD_Interrupt_Handler _endasm
}


void LCD_Init()
{

    // Set up timer 2 to generate periodic interrupt
    // The timer is driven by the CPU clock divided by 4,
    // and then by 16 (using the timer's prescaler)
    PR2 = CLOCK_FREQ / 4 / 16 / LCD_TOGGLE_RATE;

    // Timer 2 on, 1:16 prescaler, 1:1 postscaler
    T2CON = 0b00000111;

    // Enable Timer 2 Interrupt and set it
    // to high priority
    PIE1bits.TMR2IE = 1;
    IPR1bits.TMR2IP = 1;

    // Global interrupts must be enabled
    // by the caller!

    // Set the initial display type
    LCD_SetDisplayType(DISP_TYPE_WAIT);

}

void LCD_SetDigit12(unsigned char segments)
{
    if (LCD_COMMON == 1)
        segments = ~segments;

    LCD_SEG_A2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_B2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_C2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_D2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_E2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_F2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_G2 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_BC1 = segments & 0x01;

}

void LCD_SetDigit3(unsigned char segments)
{
    if (LCD_COMMON == 1)
        segments = ~segments;

    LCD_SEG_A3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_B3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_C3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_D3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_E3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_F3 = segments & 0x01;
    segments >>= 1;
    LCD_SEG_G3 = segments & 0x01;
}

void LCD_SetDisplayType(unsigned char type)
{
    LCD_DisplayType = type;

    LCD_CriticalSectionBegin();

    switch(type) {
        case DISP_TYPE_TEST:
            LCD_SetDigit12(0xff);
            LCD_SetDigit3(0xff);
            break;

        case DISP_TYPE_WAIT:
            LCD_State = 0x01;
            break;
    }

    LCD_CriticalSectionEnd();

}

void LCD_SetDisplayValue(unsigned char value)
{

}


