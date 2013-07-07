// libxser.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "win_xser.h"
#include <memory>
#include <string>
#include <regex>

using namespace std;
using namespace xser;

#define HWID_STRING L"USB\\VID_" XSER_VID L"&PID_" XSER_PID


// Default value for the instance variable
xser_instance_manager_ifx* win_xser_instance_manager::instance = 0;

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
	vector<shared_ptr<int>> z;

	// First, clean the list of current instances
	while(xser_instances.size() > 0) {
		std::tr1::shared_ptr<const xser_instance_ifx> xs = xser_instances.back();
		xser_instances.pop_back();
		xs.reset();
	}

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

	int hwid_length = wcslen(HWID_STRING);

	get_log_stream() << L"Starting device scan" << endl;

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
			printf("Device %d - Cannot obtain Hardware ID", device_index);
			continue;
		}

		get_log_stream() << L"Device " << device_index << L" (HwID=" << hardware_id << ") ... ";

		// Compare it against the desired hardware ID
		if (_wcsnicmp(HWID_STRING, hardware_id, hwid_length) == 0) {
			// Hardware ID matche
			get_log_stream() << L"Match" << endl;

			// Initialize a comm association structure
//			comm_assoc_t comm_assoc;
//			comm_assoc.port_number = -1;
//			comm_assoc.hid_path = NULL;
/*
			// Scan through the children of the device
			DWORD children_type;
			WCHAR children[1000];
			r = SetupDiGetDeviceProperty(device_info_set, &device_info_data, &DEVPKEY_Device_Children, &children_type, (PBYTE)children, 1000, NULL, 0);
			if (!r) {
				printf("  Cannot obtain children\n");
				continue;
			}

			int w;
			LPCWSTR t = (LPCWSTR)children;
			while(1) {
				w = wcslen(t);
				if (w == 0)
					break;

				wprintf(L"  Child: %s\n", t);
				process_child(world_device_info_set, t, &comm_assoc);
				t += w;
				t++;
			}

			// Check if we have all we need
			if ((comm_assoc.port_number == -1) || (comm_assoc.hid_path == NULL)) {
				printf("  Faild to obtain one or more of the required device properties\n");
				continue;
			}

			// Time do the actual work
			set_port_display((const comm_assoc_t*)&comm_assoc);
*/
		}
		else
			// Hardware ID mismatch
			get_log_stream() << L"No match" << endl;
	}
}

