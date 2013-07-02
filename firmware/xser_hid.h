/* 
 * File:   xser_hid.h
 * Author: avishay
 *
 * Created on 30 ??? 2013, 11:54
 */

#ifndef __XSER_HID_H__
#define	__XSER_HID_H__

#ifdef __18CXX
typedef unsigned char BYTE;
#else
typedef uint8_t BYTE;
#endif

/// GET_VERSION
/// -----------
///
/// Description: Retrieves the firmware version
/// Request Packet: [COMMAND]
/// Response Packet: [MAJOR] [MINOR]
#define XSER_HID_GET_VERSION      1

//typedef struct _XSER_HID_Get_Version_Response __attribute__((packed)) {
//    BYTE major;
//    BYTE minor;
//    } XSER_HID_Get_Version_Response;

/// SET_NUMBER
/// ----------
///
/// Description: Set the number displayed on the LCD
/// Request Packet: [COMMAND] [NUMBER]
/// Response Packet: - None -

#define XSER_HID_SET_NUMBER       2

/// ENTER_DFU
/// ----------
///
/// Description: Set the number displayed on the LCD
/// Request Packet: [COMMAND] [MAGIC1] [MAGIC2] [MAGIC3] [MAGIC4] ( ...zeros...)
/// Response Packet: - None -
#define XSER_HID_ENTER_DFU       3

#define XSER_HID_DFU_MAGIC1      0x34
#define XSER_HID_DFU_MAGIC2      0xa2
#define XSER_HID_DFU_MAGIC3      0x70
#define XSER_HID_DFU_MAGIC4      0xfe
    
#endif	/* __XSER_HID_H__ */

