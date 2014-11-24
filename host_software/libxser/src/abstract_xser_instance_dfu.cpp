#include <xser.h>
#include "abstract_xser_instance.h"
#include <string>
#include <stdint.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include "../../../firmware/bootloader/bootloader_protocol.h"
#include <iterator>
#include "ihex_parser.h"
#include <boost/log/trivial.hpp>

using namespace std;
using namespace xser;
using namespace ihex_parser_ns;

#define PROG_PAGE_SIZE 64

bool abstract_xser_instance_dfu::program_firmware(image_t& image, progress_callback_t report_target) const
{
	CHECK_VALIDITY;

	BOOST_LOG_TRIVIAL(debug) << "program_firmware started";

	// Make sure the image is of the correct size
	if (image.size() != FIRMWARE_SIZE)
		throw new runtime_error("Incorrect firmware image size");

	// Open the HID interface
	unique_ptr<hid_ifx> hid = get_hid_io();

	// Set the HID timeout to 1 sec
	hid->set_timeout(1000);

	if (report_target != NULL)
		report_target(0);

	rx_packet_t packet;
	memset((void*)&packet, 0, sizeof(rx_packet_t));

	int num_pages = FIRMWARE_SIZE / PROG_PAGE_SIZE;
	BOOST_LOG_TRIVIAL(debug) << "Programming " << num_pages << " pages";
	
	for(int page = 0; page < num_pages; page++) {

		// SETUP Packet
		packet.setup_packet.command = PROG_CMD_SETUP;
		packet.setup_packet.address = FIRMWARE_ADDR_START + page * PROG_PAGE_SIZE;

		for(int i = 0; i < 32; i++) {
			packet.setup_packet.data_low[i] = image[i + page*PROG_PAGE_SIZE];
		}

		hid->send_packet((uint8_t*)&packet, 64);
		if (!wait_for_response_packet(hid.get()))
			throw runtime_error("Response packet not received");

		// EXEC Packet
		packet.exec_packet.command = PROG_CMD_EXEC;
		packet.exec_packet.validation = 0xff;

		for(int i = 0; i < 32; i++) {
			packet.exec_packet.data_high[i] = image[i + page*PROG_PAGE_SIZE + 32];
		}

		hid->send_packet((uint8_t*)&packet, 64);
		if (!wait_for_response_packet(hid.get()))
			throw runtime_error("Response packet not received");

		if (report_target != NULL)
			report_target(page * 95 / num_pages);
	}

	/////// FINALIZE
	BOOST_LOG_TRIVIAL(debug) << "Programming done, finalizing";
	packet.finalize_packet.command = PROG_CMD_FINALIZE;
	packet.finalize_packet.checksum = calc_checksum((uint8_t*)image.data(), image.size());
	packet.generic_packet.command = PROG_CMD_FINALIZE;
	hid->send_packet((uint8_t*)&packet, 64);

	if (!wait_for_response_packet(hid.get()))
		throw runtime_error("Response packet not received");

	// Programming succeeded
	if (report_target != NULL)
		report_target(100);

	return true;

}

void abstract_xser_instance_dfu::reset_device()
{
	CHECK_VALIDITY;

	BOOST_LOG_TRIVIAL(debug) << "reset_device started";

	rx_packet_t packet;
	memset((void*)&packet, 0, sizeof(rx_packet_t));

	packet.reset_packet.command = PROG_CMD_RESET;
	packet.reset_packet.unlock[0] = RESET_UNLOCK_0;
	packet.reset_packet.unlock[1] = RESET_UNLOCK_1;
	packet.reset_packet.unlock[2] = RESET_UNLOCK_2;
	packet.reset_packet.unlock[3] = RESET_UNLOCK_3;

	get_hid_io()->send_packet((uint8_t*)&packet, 64);

	// No response to expect ...
}

uint16_t abstract_xser_instance_dfu::calc_checksum(uint8_t* buf, unsigned int size) const
{
	uint16_t checksum = 0;
	for(int i = 0; i < size; i++)
		checksum += (unsigned char)buf[i];

	return checksum;
}

bool abstract_xser_instance_dfu::wait_for_response_packet(const hid_ifx* hid) const
{
	shared_ptr<char> r = hid->receive_packet();
	return (r.get()[1] != 0);
}
