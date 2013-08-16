#ifndef __HARDWAREPROFILE_H__
#define __HARDWAREPROFILE_H__

/*******************************************************************/
/******** USB stack hardware selection options *********************/
/*******************************************************************/

/** Pin Mapping ************************************************************/
//    Pin Name    Dir      Function
//    --------    ---      -------------
//      RA0       IN       Logic Level Detect
//      RA1       OUT      LCD Common
//      RA2       OUT      LCD D2
//      RA3       OUT      LCD E2
//      RA4       OUT      LCD G2
//      RA5       OUT      LCD F2
//      RA6       OUT      LCD B2
//      RA7       OUT      LCD A2
//
//      RB0       OUT      LCD A3
//      RB1       OUT      LCD B3
//      RB2       OUT      LCD C3
//      RB3       OUT      LCD D3
//      RB4       OUT      LCD E3
//      RB5       OUT      LCD G3
//      RB6       OUT      LCD F3
//      RB7       OUT      LCD BC1
//
//      RC0       OUT      LCD C2
//      RC1       OUT      RTS
//      RC2       IN       CTS
//      RC3       --       ----
//      RC4       --       USB D-
//      RC5       --       USB D+
//      RC6       OUT      Serial TX
//      RC7       IN       Serial RX
//
//      RE3       IN       Pushbutton


#define ROM rom

// Set the Tri-State registers according to the
// pin function table
#define IO_Init() \
        TRISA = 0b00000001; \
        TRISB = 0b00000000; \
        TRISC = 0b11011100;

// Device is bus powered
#define self_power 0

#define CLOCK_FREQ 16000000
#define GetSystemClock() CLOCK_FREQ   

/** LCD ************************************************************/
#define LCD_SEG_BC1  LATBbits.LATB7
#define LCD_SEG_A2   LATAbits.LATA7
#define LCD_SEG_B2   LATAbits.LATA6
#define LCD_SEG_C2   LATCbits.LATC0
#define LCD_SEG_D2   LATAbits.LATA2
#define LCD_SEG_E2   LATAbits.LATA3
#define LCD_SEG_F2   LATAbits.LATA5
#define LCD_SEG_G2   LATAbits.LATA4
#define LCD_SEG_A3   LATBbits.LATB0
#define LCD_SEG_B3   LATBbits.LATB1
#define LCD_SEG_C3   LATBbits.LATB2
#define LCD_SEG_D3   LATBbits.LATB3
#define LCD_SEG_E3   LATBbits.LATB4
#define LCD_SEG_F3   LATBbits.LATB6
#define LCD_SEG_G3   LATBbits.LATB5
#define LCD_COMMON   LATAbits.LATA1

// Define DTS and DTR pins (see usb_function_cdc.h)
#define UART_DTS     PORTCbits.RC2
#define UART_DTR     PORTCbits.RC1
#define mInitDTSPin()
#define mInitDTRPin()


// Masks defining the bits related to the LCD
// in each port
#define RA_LCD_BITS  0b11111110
#define RB_LCD_BITS  0b11111111
#define RC_LCD_BITS  0b00000001

#define LCD_Invert_Bits() \
  LATA = LATA ^ RA_LCD_BITS; \
  LATB = LATB ^ RB_LCD_BITS; \
  LATC = LATC ^ RC_LCD_BITS;

// The toggle rate of the LCD outputs
#define LCD_TOGGLE_RATE 10000

// USB Stack configuration
//#define USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL

// Debugging options
//#define DBG_SERIAL_LOOPBACK

#endif



