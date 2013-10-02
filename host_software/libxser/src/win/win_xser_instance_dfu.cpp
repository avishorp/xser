#include "stdafx.h"
#include <xser.h>
#include <string>
#include <stdint.h>
#include "win_xser_instance_dfu.h"
#include "win_hid_ifx.h"

using namespace std;
using namespace xser;

win_xser_instance_dfu::win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data)
{
	// Set default member variables
	serial_number = serial;

	// Scan through the children of the device. This function returns a list of null-separated
	// childrens. It is expected that this kind of device has only one child
	DWORD children_type;
	WCHAR children[1000];
	BOOLEAN r = SetupDiGetDeviceProperty(dev_info_set, dev_info_data, &DEVPKEY_Device_Children, &children_type, (PBYTE)children, 1000, NULL, 0);
	if (!r)
		throw runtime_error("Cannot obtain children");

	hid_io = win_hid_ifx::from_child(children);

}

win_xser_instance_dfu::~win_xser_instance_dfu()
{
}
