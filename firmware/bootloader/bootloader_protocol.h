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
#define PROG_CMD_RESET      0x04

// Reset unlock code
#define RESET_UNLOCK_0      0x78
#define RESET_UNLOCK_1      0xa0
#define RESET_UNLOCK_2      0x23
#define RESET_UNLOCK_3      0x01

#ifdef __18CXX
#define UINT8   byte
#define UINT16  WORD
#define UINT32  unsigned long
#elif defined(_MSC_VER)
#define UINT8  uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t
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
    UINT8   padding[64 - 1 - 2];
} packet_finalize_t;

typedef struct {
    UINT8   command;
    UINT8   unlock[4];
    UINT8   padding[64 - 1 - 4];
} packet_reset_t;

typedef union {
    packet_generic_t  generic_packet;
    packet_setup_t    setup_packet;
    packet_exec_t     exec_packet;
    packet_finalize_t finalize_packet;
    packet_reset_t    reset_packet;
} rx_packet_t;

typedef struct {
    UINT8   result;
    UINT8   padding[63];
} response_packet_t;


#endif	/* BOOTLOADER_PROTOCOL_H */

