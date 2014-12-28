// Internal osccilator, no ouput to a pin
#pragma config FOSC = INTOSCIO



// Enable the PLL x3 to provide 48MHz to the USB core
// The software has to set the internal clock post-scaler output to 16MHz
#pragma config PLLSEL = PLL3X
#pragma config CFGPLLEN = ON

// The system runs on 16MHz, thus a /3 divider is selected
#pragma config CPUDIV = NOCLKDIV

// Enable the primaty oscillator
#pragma config PCLKEN = ON

// Fail-safe clock monitor is not required
#pragma config FCMEN = OFF

// Disable oscillator switch-over
#pragma config IESO = OFF

// Disable BOR
#pragma config BOREN = OFF

// Disable watchdog
#pragma config WDTEN = NOSLP
#pragma config WDTPS = 2048

// Disable the A/D on PORTB
#pragma config PBADEN = OFF

// MCLR is used as an I/O pin
#pragma config MCLRE = OFF

// Single supply ICSP is not required
#pragma config LVP = OFF

// Disable extended instruction set
#pragma config XINST = ON

// Protect the configuration bits
#pragma config WRTC = ON
