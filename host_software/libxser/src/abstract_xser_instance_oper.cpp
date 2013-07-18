
#include <xser.h>
#include <string>
#include <stdint.h>
#include "../../../firmware/xser_hid.h"

using namespace std;
using namespace xser;


void abstract_xser_instance_oper::set_com_display(int number) const {
	
	const hid_ifx& h = get_hid_io();

	// Create the request buffer
	int8_t req_buf[64];
	memset(req_buf, 0, sizeof(req_buf));
	req_buf[0] = XSER_HID_SET_NUMBER;

	h.send_packet(req_buf, sizeof(req_buf));

}

void abstract_xser_instance_oper::get_firmware_version(int& major, int& minor) const {
}

void abstract_xser_instance_oper::enter_dfu() const {
}




