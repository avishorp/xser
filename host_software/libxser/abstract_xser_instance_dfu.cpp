#include <xser.h>
#include <string>
#include <stdint.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include "../../firmware/bootloader/bootloader_protocol.h"
#include <iterator>
#include "ihex_parser.h"

using namespace std;
using namespace xser;
using namespace ihex_parser_ns;

#define MIN_PROG_ADDRESS 0x1000
#define PROG_SIZE        0x3000

void abstract_xser_instance_dfu::program_firmware(void* image, int len) const
{
	ihex_parser ihx(MIN_PROG_ADDRESS, MIN_PROG_ADDRESS + PROG_SIZE - 1);

	ifstream inf;
	inf.open("U:\\projects\\xser\\firmware\\xser\\dist\\default\\production\\xser.production.hex", ifstream::in);

	ihx.parse(inf);
	image = (void*)ihx.get_buffer();

	rx_packet_t packet;

	int num_pages = PROG_SIZE / 16;
	

	for(int page = 0; page < num_pages; page++) {

		// SETUP Packet
		packet.setup_packet.command = PROG_CMD_SETUP;
		packet.setup_packet.address = MIN_PROG_ADDRESS + page * 16;

		for(int i = 0; i < 32; i++) {
			packet.setup_packet.data_low[i] = ((uint8_t*)image)[i + page*16];
		}

		get_hid_io().send_packet((uint8_t*)&packet, 64);

		// EXEC Packet
		packet.exec_packet.command = PROG_CMD_EXEC;
		packet.exec_packet.validation = 0xff;

		for(int i = 0; i < 32; i++) {
			packet.exec_packet.data_high[i] = ((uint8_t*)image)[i + page*16 + 16];
		}

		get_hid_io().send_packet((uint8_t*)&packet, 64);
	}

}

void abstract_xser_instance_dfu::finalize_programming() const
{
}
