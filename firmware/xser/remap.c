/* 
 * File:   remap.c
 * Author: avishay
 *
 * Created on 5 ?????? 2013, 16:23
 */

#define REMAPPED_RESET_VECTOR_ADDRESS		0x1000
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x1008
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x1018


extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
void _reset (void)
{
    _asm goto _startup _endasm
}

extern void LCD_Interrupt_Handler();

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR (void)
{
     _asm goto LCD_Interrupt_Handler _endasm
}

//#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
//void Remapped_Low_ISR (void)
//{
//     _asm goto YourLowPriorityISRCode _endasm
//}

