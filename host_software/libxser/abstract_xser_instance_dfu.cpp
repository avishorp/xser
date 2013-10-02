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

#define PROG_PAGE_SIZE 64

void abstract_xser_instance_dfu::program_firmware(image_t& image, progress_callback_t report_target) const
{
	// Make sure the image is of the correct size
	if (image.size() != FIRMWARE_SIZE)
		throw new runtime_error("Incorrect firmware image size");

	if (report_target != NULL)
		report_target(0);

	rx_packet_t packet;
	int num_pages = FIRMWARE_SIZE / PROG_PAGE_SIZE;
	
	for(int page = 0; page < num_pages; page++) {

		// SETUP Packet
		packet.setup_packet.command = PROG_CMD_SETUP;
		packet.setup_packet.address = FIRMWARE_ADDR_START + page * PROG_PAGE_SIZE;

		for(int i = 0; i < 32; i++) {
			packet.setup_packet.data_low[i] = image[i + page*PROG_PAGE_SIZE];
		}

		get_hid_io().send_packet((uint8_t*)&packet, 64);

		// EXEC Packet
		packet.exec_packet.command = PROG_CMD_EXEC;
		packet.exec_packet.validation = 0xff;

		for(int i = 0; i < 32; i++) {
			packet.exec_packet.data_high[i] = image[i + page*PROG_PAGE_SIZE + 32];
		}

		get_hid_io().send_packet((uint8_t*)&packet, 64);

		if (report_target != NULL)
			report_target(page * 95 / num_pages);
	}

	/////// FINALIZE
	packet.generic_packet.command = PROG_CMD_FINALIZE;
	get_hid_io().send_packet((uint8_t*)&packet, 64);

	if (report_target != NULL)
		report_target(100);

}

void abstract_xser_instance_dfu::finalize_programming() const
{
}
