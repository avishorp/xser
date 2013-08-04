
#include "stdafx.h"
#include <xser.h>
#include "win_xser_instance_oper.h"
#include "win_hid_ifx.h"

#include <regex>

using namespace std;

// A regular expression for searching the COMx number
std::tr1::basic_regex<WCHAR> regex_com(L"COM([0-9]+)");

win_xser_instance_oper::win_xser_instance_oper(string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data)
{
	// Set default member variables
	serial_number = serial;
	com_number = -1;
	hid_io = NULL;

	// Prepare an info set of all the device. It will be used later
	// when device children are processed
	HDEVINFO world_device_info_set = INVALID_HANDLE_VALUE;
	world_device_info_set = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT);

	if (world_device_info_set == INVALID_HANDLE_VALUE) 
		throw runtime_error("Could not get all device information set");


	// Scan through the children of the device. This function returns a list of null-separated
	// childrens
	DWORD children_type;
	WCHAR children[1000];
	BOOLEAN r = SetupDiGetDeviceProperty(dev_info_set, dev_info_data, &DEVPKEY_Device_Children, &children_type, (PBYTE)children, 1000, NULL, 0);
	if (!r)
		throw runtime_error("Cannot obtain children");

	int w;
	LPCWSTR t = (LPCWSTR)children;
	while(1) {
		w = wcslen(t);
		if (w == 0)
			// End of list
			break;

		process_child(world_device_info_set, t);
		t += w;
		t++;
	}

	// Check that after all children has been processes, we have all we
	// need
	if ((com_number == -1) || (hid_io == NULL))
		throw runtime_error("Incorrect USB device structure");
}

void win_xser_instance_oper::process_child(HDEVINFO world_device_info_set, LPCWSTR child_id)
{
	// Get the device according to its instance ID
	BOOLEAN r;
	SP_DEVINFO_DATA device_info_data;
	memset(&device_info_data, 0x0, sizeof(device_info_data));
	device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);
	r = SetupDiOpenDeviceInfo(world_device_info_set, child_id, NULL, 0, &device_info_data);

	if (!r) 
		throw runtime_error("Could not find the child device");

	// Determine the device class
	WCHAR class_name[300];
	DWORD class_name_type;
	r = SetupDiGetDeviceProperty(world_device_info_set, &device_info_data, &DEVPKEY_Device_Class, &class_name_type, (PBYTE)class_name, 300, NULL, 0);

	if (!r)
		throw runtime_error("Could not obtain the device class");

	if (wcscmp(class_name, L"HIDClass") == 0) {
		// Process the HID side

		// The HID device is actually the child of this device
		DWORD children_type;
		WCHAR children[1000];
		r = SetupDiGetDeviceProperty(world_device_info_set, &device_info_data, &DEVPKEY_Device_Children, &children_type, (PBYTE)children, 1000, NULL, 0);
		if (!r) 
			throw runtime_error("Could not get the child device instance");

		// Open the child device.
		// I don't know why I have to create a new device information set to enumerate its HID
		// interface, but that's the only way it works.
		HDEVINFO hid_device_info_set = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		r = SetupDiOpenDeviceInfo(hid_device_info_set, children, NULL, 0, &device_info_data);
		if (!r) 
			throw runtime_error("Could not open the child HID device");

		// Enumarate through all device interfaces
		SP_DEVICE_INTERFACE_DATA device_interface_data;
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		for(int k = 0;; k++) {
			// Get the interface data
			r = SetupDiEnumDeviceInterfaces(hid_device_info_set, &device_info_data, &GUID_DEVINTERFACE_HID, k, &device_interface_data);
		
			if (!r)
				// No more interfaces
				break;

			// Get the device interface detail
			SP_DEVICE_INTERFACE_DETAIL_DATA_W* detail;
			DWORD detail_size = 0;

			// First call to get the required size
			r = SetupDiGetDeviceInterfaceDetail(hid_device_info_set, &device_interface_data, NULL, 0, &detail_size, &device_info_data);

			if (detail_size > 0) {
				// Allocate data for the detail
				detail = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)malloc(detail_size);
				detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

				// Call SetupDiGetDeviceInterfaceDetail again, to get the actual data
				r = SetupDiGetDeviceInterfaceDetail(hid_device_info_set, &device_interface_data, detail, detail_size, NULL, &device_info_data);

				if (!r) {
					free(detail);
					continue;
				}

				// Create a hid_ifx object from the device path
				hid_io = new win_hid_ifx(detail->DevicePath);

			}
		}

	}
	else if (wcscmp(class_name, L"Ports") == 0) {
		// Process the COM port side
		WCHAR desc[300];
		DWORD desc_type;
		r = SetupDiGetDeviceProperty(world_device_info_set, &device_info_data, &DEVPKEY_Device_FriendlyName, &desc_type, (PBYTE)desc, 300, NULL, 0);
		if (!r)
			throw runtime_error("Could not obtain port description");

		std::tr1::wcmatch mr;
		bool match_result = std::tr1::regex_search<WCHAR>(desc, mr, regex_com);
		if (match_result)
			com_number = _wtoi(mr[1].str().c_str());
	}
	else {
//		printf("    Unknown child type\n");
//		return false;
	}
}

win_xser_instance_oper::~win_xser_instance_oper()
{
	if (hid_io != NULL)
		delete hid_io;
}


