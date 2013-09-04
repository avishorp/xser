/* 
 * File:   bootloader_protocol.h
 * Author: avishay
 *
 * Created on 30 ?????? 2013, 23:21
 */

#ifndef BOOTLOADER_PROTOCOL_H
#define	BOOTLOADER_PROTOCOL_H

#define LOW_PROG_ADDRESS    0x1000  // The lowest & highest addresses
#define HIGH_PROG_ADDRESS   0x3fff  // that can be programmed

#define PROG_CMD_SETUP      0x01
#define PROG_CMD_EXEC       0x02
#define PROG_CMD_FINALIZE   0x03

#ifdef __18CXX
#define UINT8   byte
#define UINT16  WORD
#elif defined(_MSC_VER)
#define UINT8  uint8_t
#define UINT16 uint16_t
#endif

#ifdef _MSC_VER
#pragma pack(1)
#endif
typedef struct {
    UINT8   command;
    UINT8   padding[64 - 1];
} packet_generic_t;

typedef struct {
    UINT8   command;
    UINT16  address;
    UINT8   data_low[32];
    UINT8   padding[64 - 1 - 2 - 32];
} packet_setup_t;

typedef struct {
    UINT8   command;
    UINT8   validation;
    UINT8   data_high[32];
    UINT8   padding[64 - 1 - 1 - 32];
} packet_exec_t;

typedef struct {
    UINT8   command;
    UINT16  checksum;
} packet_finalize_t;

typedef union {
    packet_generic_t  generic_packet;
    packet_setup_t    setup_packet;
    packet_exec_t     exec_packet;
    packet_finalize_t finalize_packet;
} rx_packet_t;


#endif	/* BOOTLOADER_PROTOCOL_H */

