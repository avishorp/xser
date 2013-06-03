/* 
 * File:   xser_hid.h
 * Author: avishay
 *
 * Created on 30 ??? 2013, 11:54
 */

#ifndef XSER_HID_H
#define	XSER_HID_H

/// GET_VERSION
/// -----------
///
/// Description: Retrieves the firmware version
//  Request Packet: [COMMAND]
//  Response Packet: [MAJOR] [MINOR]
#define XSERHID_GET_VERSION      1

typedef struct __attribute__((packed)) {
    unsigned char major;
    unsigned char minor;
    } XSERHID_Get_Version_Response;

/// SET_NUMBER
/// ----------
///
/// Description: Set the number displayed on the LCD
//  Request Packet: [COMMAND] [NUMBER]
//  Response Packet: - None -

#define XSERHID_SET_NUMBER       2

#endif	/* XSER_HID_H */

