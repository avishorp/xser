/* 
 * File:   hid_protocol.h
 * Author: avishay
 *
 * Created on 4 ??????? 2013, 19:45
 */

#ifndef HID_PROTOCOL_H
#define	HID_PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef __18CXX
#include <GenericTypeDefs.h>
#elif defined(_MSC_VER)
#define UINT8  uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t
#endif

// HID Commands
///////////////    

/// GET_VERSION
/// -----------
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

// DFU Unlock Key
/////////////////
#define DFU_UNLOCK_0      0x34
#define DFU_UNLOCK_1      0xa2
#define DFU_UNLOCK_2      0x70
#define DFU_UNLOCK_3      0xfe

#ifdef _MSC_VER
#pragma pack(1)
#endif

typedef struct {
    UINT8   command;
    UINT8   padding[64 - 1];
} packet_generic_t;

typedef struct {
    UINT8   command;
    UINT8   padding[64 - 1];
} packet_get_version_t;

typedef struct {
    UINT8   minor;
    UINT8   major;
} response_get_version_t;

typedef struct {
    UINT8   command;
    UINT8   com_number;
    UINT8   padding[64 - 2];
} packet_set_com_t;

typedef struct {
    UINT8   command;
    UINT8   unlock[4];
    UINT8   padding[64 - 1 - 4];
} packet_enter_dfu_t;

typedef union {
    packet_generic_t    generic_packet;
    packet_set_com_t    set_com_packet;
    packet_enter_dfu_t  enter_dfu_packet;
} request_packet_t;


#ifdef	__cplusplus
}
#endif

#endif	/* HID_PROTOCOL_H */

