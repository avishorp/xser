#include "stdafx.h"
#include "win_hid_ifx.h"
#include "win_exception.h"
#include <sstream>

using namespace std;
using namespace xser;

win_hid_ifx::win_hid_ifx(const wchar_t* instance_path_)
{
	hid_handle = INVALID_HANDLE_VALUE;
	instance_path = instance_path_;
	open();
}

win_hid_ifx::~win_hid_ifx()
{
	close();
}

void win_hid_ifx::open()
{
	// Open the HID interface for read and write
	hid_handle = CreateFile(instance_path.c_str(), (GENERIC_WRITE|GENERIC_READ), FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hid_handle == INVALID_HANDLE_VALUE) {
		char* message = new char[100];
		sprintf(message, "Failed opening the HID interface (%d)", GetLastError());
		throw std::runtime_error(message);
	}

}

void win_hid_ifx::close()
{
	if (hid_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(hid_handle);
		hid_handle = INVALID_HANDLE_VALUE;
	}
}

void win_hid_ifx::send_packet(uint8_t* packet, int len) const
{
	// Make sure that the length of the message is grater than zero and smaller or
	// equal to 64
	if ((len < 1) || (len > 64))
		throw std::runtime_error("Illegal HID message size");

	// Prepare the message - 65 bytes in which the first
	// is the report number (0) and the rest is the message contents.
	// All the other bytes are zero.
	char message[65];
	memset(message, 0, 65);
	memcpy(message+1, packet, len);
	
	DWORD num_written;
	BOOLEAN r = WriteFile(hid_handle, message, 65, &num_written, NULL);

	if (!r) {
			WIN_API_THROW("Write failed");
	}
}


std::shared_ptr<char> win_hid_ifx::receive_packet() const
{
	char* buf = new char[65];
	memset(buf, 0, 65);
	DWORD num_read;
	BOOLEAN r = ReadFile(hid_handle, buf, 65, &num_read, NULL);

	if (r) {
		return shared_ptr<char>(buf);
	}
	else {
		DWORD err = GetLastError();
		return 0;
	}
}

void win_hid_ifx::set_timeout(int to) const 
{
	// TODO: This doesn't work, have to switch to overlapped I/O
	COMMTIMEOUTS tos;
	memset(&tos, 0, sizeof(COMMTIMEOUTS));
	tos.ReadIntervalTimeout = to;
	tos.ReadTotalTimeoutMultiplier = 1;
	tos.WriteTotalTimeoutConstant = to;
	SetCommTimeouts(hid_handle, &tos);
}

wstring win_hid_ifx::from_child(LPCWSTR instance_id)
{
	// Prepare an info set of all the device. It will be used later
	// when device children are processed
	HDEVINFO world_device_info_set = INVALID_HANDLE_VALUE;
	world_device_info_set = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT);

	if (world_device_info_set == INVALID_HANDLE_VALUE) 
		throw runtime_error("Could not get all device information set");

	// Create a device_info_data structure
	BOOLEAN r;
	SP_DEVINFO_DATA device_info_data;
	memset(&device_info_data, 0x0, sizeof(device_info_data));
	device_info_data.cbSize = sizeof(SP_DEVINFO_DATA);

	// Open the child device.
	// I don't know why I have to create a new device information set to enumerate its HID
	// interface, but that's the only way it works.
	HDEVINFO hid_device_info_set = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	r = SetupDiOpenDeviceInfo(hid_device_info_set, instance_id, NULL, 0, &device_info_data);
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

			return detail->DevicePath;
		}
	}

	// If wer'e at this point, we coudn't find the HID interface
	throw runtime_error("Could not find a HID interface");
}

