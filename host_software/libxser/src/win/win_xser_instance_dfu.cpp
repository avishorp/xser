#include "stdafx.h"
#include <xser.h>
#include <string>
#include <sstream>
#include <stdint.h>
#include "win_xser_instance_dfu.h"
#include "win_hid_ifx.h"
#include "win_exception.h"

using namespace std;
using namespace xser;

win_xser_instance_dfu::win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data,
											 physical_location_t& physical_loc_)
{
	// Set default member variables
	serial_number = serial;
	valid = false;
	physical_loc = physical_loc_;

	// Scan through the children of the device. This function returns a list of null-separated
	// childrens. It is expected that this kind of device has only one child
	DWORD children_type;
	WCHAR children[1000];
	BOOLEAN r = SetupDiGetDeviceProperty(dev_info_set, dev_info_data, &DEVPKEY_Device_Children, &children_type, (PBYTE)children, 1000, NULL, 0);
	if (!r) {
		WIN_API_THROW("Cannot obtain children");
	}

	hid_io = win_hid_ifx::from_child(children);

	// Make the object valid
	valid = true;
}

win_xser_instance_dfu::~win_xser_instance_dfu()
{
	abstract_xser_instance_dfu::~abstract_xser_instance_dfu();
}

void win_xser_instance_dfu::invalidate()
{
	valid = false;
}

void win_xser_instance_dfu::connect()
{
	CHECK_VALIDITY;

	win_hid_ifx& h = dynamic_cast<win_hid_ifx&>(get_hid_io());
	h.open();
	
}

void win_xser_instance_dfu::disconnect()
{
	win_hid_ifx& h = dynamic_cast<win_hid_ifx&>(get_hid_io());
	h.close();

	// Invalidate the object
	valid = false;

}

hid_ifx& win_xser_instance_dfu::get_hid_io() const { 
	CHECK_VALIDITY;
	return *hid_io; 
}

const std::string& win_xser_instance_dfu::get_serial_number() const {
	CHECK_VALIDITY;
	return serial_number; 
}

const physical_location_t& win_xser_instance_dfu::get_physical_location() const { 
	CHECK_VALIDITY;
	return physical_loc;
}

void win_xser_instance_dfu::reset_device()
{
	abstract_xser_instance_dfu::reset_device();
	disconnect();
}



