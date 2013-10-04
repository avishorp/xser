
#include <xser.h>
#include <string>
#include <stdint.h>
#include "../../../firmware/xser/hid_protocol.h"

using namespace std;
using namespace xser;


void abstract_xser_instance_oper::set_com_display(int number) const {
	
	const hid_ifx& h = get_hid_io();

	// Create the request buffer
	uint8_t req_buf[2];
	memset(req_buf, 0, sizeof(req_buf));
	req_buf[0] = XSER_HID_SET_NUMBER;
	req_buf[1] = number;

	h.send_packet(req_buf, sizeof(req_buf));

}

void abstract_xser_instance_oper::get_firmware_version(int& major, int& minor) const {
}

void abstract_xser_instance_oper::enter_dfu() const {
	// Create a request buffer
	packet_enter_dfu_t req;
	memset(&req, 0, sizeof(req));

	req.command = XSER_HID_ENTER_DFU;
	req.unlock[0] = DFU_UNLOCK_0;
	req.unlock[1] = DFU_UNLOCK_1;
	req.unlock[2] = DFU_UNLOCK_2;
	req.unlock[3] = DFU_UNLOCK_3;

	// Send the request
	const hid_ifx& h = get_hid_io();
	h.send_packet((uint8_t*)&req, sizeof(req));

	// No response to wait for
}




