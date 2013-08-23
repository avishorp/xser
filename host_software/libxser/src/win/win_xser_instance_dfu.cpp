#include "stdafx.h"
#include <xser.h>
#include <string>
#include <stdint.h>
#include "win_xser_instance_dfu.h"

using namespace std;
using namespace xser;

win_xser_instance_dfu::win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data)
{
	// Set default member variables
	serial_number = serial;
	hid_io = NULL;
}

win_xser_instance_dfu::~win_xser_instance_dfu()
{
	if (hid_io != NULL)
		delete hid_io;
}
