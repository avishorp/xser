// libxser.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "win_xser.h"
#include "win_xser_instance_oper.h"
#include "win_xser_instance_dfu.h"
#include "win_exception.h"
#include <memory>
#include <string>
#include <regex>
#include <sstream>


using namespace std;
using namespace xser;

#define HWID_OPER_STRING L"USB\\VID_" XSER_VID L"&PID_" XSER_PID
#define HWID_DFU_STRING  L"USB\\VID_" XSER_VID L"&PID_" XSER_PID_DFU


// Default value for the instance variable
xser_instance_manager_ifx* win_xser_instance_manager::instance = 0;

// Implementation of the platform-dependent helper function defined in xser.h
EXPORT xser_instance_manager_ifx& xser::win_xser_get_instance_manager()
{
	return win_xser_instance_manager::get_instance();
}


xser_instance_manager_ifx& win_xser_instance_manager::get_instance()
{
	if (!instance)
		instance = new win_xser_instance_manager();

	return *instance;
}

win_xser_instance_manager::win_xser_instance_manager() : xser_instances()
{
}

const xser_instances_t& win_xser_instance_manager::get_xser_instances() const 
{
	return xser_instances;
}

void win_xser_instance_manager::rescan()
{
	// Scan all the USB devices connected to the computer
	HDEVINFO device_info_set = INVALID_HANDLE_VALUE;
	device_info_set = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (device_info_set == INVALID_HANDLE_VALUE) {
		throw runtime_error("Could not get device information set for all USB devices");
	}

	// Prepare an info set of all the device. It will be used later
	// when device children are processed
	HDEVINFO world_device_info_set = INVALID_HANDLE_VALUE;
	world_device_info_set = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT);
	//world_device_info_set = SetupDiGetClassDevsA(&GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (world_device_info_set == INVALID_HANDLE_VALUE) {
		throw runtime_error("Could not get all device information set");
	}

	// Do the actual scan
	SP_DEVINFO_DATA device_info_data;
	memset(&device_info_data, 0x0, sizeof(device_info_data));
	device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
	BOOLEAN r;
	int working_device_count = 0;
	xser_instance_ifx* xsi = NULL;

	int hwid_oper_length = wcslen(HWID_OPER_STRING);
	int hwid_dfu_length = wcslen(HWID_DFU_STRING);

	get_verbose_stream() << "Starting device scan" << endl;

	for(int device_index = 0; ; device_index++) {

		// Get the device info data of the next device
		r = SetupDiEnumDeviceInfo(device_info_set, device_index, &device_info_data);
		if (!r)
			// End of lins
			break;

		// Obtain the HardrareIDs of the device
		wchar_t hardware_id[300];
		DWORD hardware_id_type;
		r = SetupDiGetDeviceProperty(device_info_set, &device_info_data, &DEVPKEY_Device_HardwareIds, &hardware_id_type, (PBYTE)hardware_id, 300, NULL, 0);
		if (!r) {
			get_verbose_stream() << "Device " << device_index << " - Cannot obtain Hardware ID" << endl;
			continue;
		}

		wstring hardware_id_wstr(hardware_id);
		string hardware_id_str(hardware_id_wstr.begin(), hardware_id_wstr.end());
		get_verbose_stream() << "Device " << device_index << " (HwID=" << hardware_id_str << ") ... ";

		// Compare it against the desired hardware ID
		if (_wcsnicmp(HWID_OPER_STRING, hardware_id, hwid_oper_length) == 0) {
			// Hardware ID match (operational)
			get_verbose_stream() << "Match" << endl;

			// Get the serial number
			auto_ptr<string> serial = get_serial_number(device_info_set, &device_info_data);

			// Get the physical location
			string phy = get_location(device_info_set, &device_info_data);

			// Create a new xser instance object
			xsi = new win_xser_instance_oper(*serial, device_info_set, &device_info_data, phy);
		}
		else if (_wcsnicmp(HWID_DFU_STRING, hardware_id, hwid_dfu_length) == 0)
		{                            
			// Hardware ID match (operational)
			get_verbose_stream() << "Match (DFU)" << endl;

			// Get the serial number
			auto_ptr<string> serial = get_serial_number(device_info_set, &device_info_data);

			// Get the physical location
			string phy = get_location(device_info_set, &device_info_data);

			// Create a new DFU xser instance object
 			xsi = new win_xser_instance_dfu(*serial, device_info_set, &device_info_data, phy);
		}
		else
			// Hardware ID mismatch
			get_verbose_stream() << "No match" << endl;

		if (xsi != NULL) {
			// Check if the instance is not already on the list
			xser_instances_iter_t it = xser_instances.find(xsi->get_physical_location());

			if (it == xser_instances.end()) {
				// No item in the physical location, insert it
				xser_instances[xsi->get_physical_location()] = xsi;
				xsi->connect();
			}
			else {
				// There is already an item in the physical location
				if ((it->second->get_serial_number() == xsi->get_serial_number()) &&
					(it->second->is_dfu_mode() == xsi->is_dfu_mode())) {

						// The newly created object is identical to the one already
						// on the list. Discard the new object.
						delete xsi;
				}
				else {
					// The item on the list in the physical location is not identical to
					// the new object. Discard the old one and replace it by the new one.
					delete it->second;
					xser_instances[xsi->get_physical_location()] = xsi;
					xsi->connect();
				}

			}
		}
	}
}

std::auto_ptr<string> win_xser_instance_manager::get_serial_number(HDEVINFO device_info_set, PSP_DEVINFO_DATA device_info_data)
{
	// Obtain the serial number of the device
	DWORD instance_id_size;
	BOOLEAN r = SetupDiGetDeviceInstanceId(device_info_set, device_info_data, NULL, 0, &instance_id_size);
	wchar_t* instance_id = new wchar_t[instance_id_size + 1];

	r = SetupDiGetDeviceInstanceId(device_info_set, device_info_data, instance_id, instance_id_size, NULL);
	if (!r)
		throw runtime_error("Could not get all device instance ID");

	wstring instance_id_wstr(instance_id);
	string instance_id_str(instance_id_wstr.begin(), instance_id_wstr.end());

	int k = instance_id_str.rfind('\\');
	auto_ptr<string> ret(new string(instance_id_str.substr(k + 1, instance_id_str.length() - k - 1)));

	get_verbose_stream() << ">> " << *ret << endl;

	return ret;
}

string win_xser_instance_manager::get_location(HDEVINFO device_info_set, PSP_DEVINFO_DATA device_info_data)
{
	wchar_t physical_buf[1000];
	DEVPROPTYPE type;
	DWORD size_retrieved;
	BOOLEAN r = SetupDiGetDeviceProperty(device_info_set, device_info_data, &DEVPKEY_Device_LocationPaths, &type, (PBYTE)physical_buf, 
		sizeof(physical_buf), &size_retrieved, 0);

	if (!r) {
		WIN_API_THROW("Could not get physical location");
	}

	wstring physical_loc_wstr(physical_buf);
	string physical_loc_str(physical_loc_wstr.begin(), physical_loc_wstr.end());

	return physical_loc_str;
}

