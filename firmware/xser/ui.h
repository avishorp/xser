/* 
 * File:   ui.h
 * Author: avishay
 *
 * Created on 13 ???? 2013, 10:52
 */

#ifndef UI_H
#define	UI_H

// UI Related Events
#define EVENT_RX_ACT      0x01  // RX Activity
#define EVENT_TX_ACT      0x02  // TX Activity
#define EVENT_BTN_PUSHED  0x04  // Button pressed
#define EVENT_USBCONF     0x08  // USB went into configured state
#define EVENT_HIDCMD      0x10  // HID Command to set port number
#define EVENT_TIMEOUT     0x80  // Timeout (internal event)

void UI_Init();
void UI_Service(unsigned char events);
void UI_SetPortNumber(unsigned char portnum);

#endif	/* UI_H */

