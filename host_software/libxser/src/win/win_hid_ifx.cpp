#include "stdafx.h"
#include "win_hid_ifx.h"

win_hid_ifx::win_hid_ifx(wchar_t* instance_path)
{
	// Open the HID interface for read and write
	hid_handle = CreateFile(instance_path, (GENERIC_WRITE|GENERIC_READ), FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hid_handle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed opening the HID interface");
	}
}

win_hid_ifx::~win_hid_ifx()
{
	if (hid_handle != INVALID_HANDLE_VALUE)
		CloseHandle(hid_handle);
}

void win_hid_ifx::send_packet(int8_t* packet, int len) const
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

	if (!r)
		throw std::runtime_error("Write failed");
}



std::shared_ptr<char> win_hid_ifx::receive_packet() const
{
	return 0;
}
