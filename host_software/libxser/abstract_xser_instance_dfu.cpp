#include <xser.h>
#include <string>
#include <stdint.h>
#include <memory.h>
#include "../../firmware/bootloader/bootloader_protocol.h"

using namespace std;
using namespace xser;


void abstract_xser_instance_dfu::program_firmware(void* image, int len) const
{
	rx_packet_t packet;

	packet.setup_packet.command = PROG_CMD_SETUP;
	packet.setup_packet.address = 0x1300;
	
	for(int i = 0; i < 32; i++) {
		packet.setup_packet.data_low[i] = i + 16;
	}

	get_hid_io().send_packet((uint8_t*)&packet, 64);

	packet.exec_packet.command = PROG_CMD_EXEC;
	packet.exec_packet.validation = 0xff;

	for(int i = 0; i < 32; i++) {
		packet.exec_packet.data_high[i] = i + 16 + 32;
	}

	get_hid_io().send_packet((uint8_t*)&packet, 64);


}

void abstract_xser_instance_dfu::finalize_programming() const
{
}
