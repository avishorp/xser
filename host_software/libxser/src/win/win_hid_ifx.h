#include <xser.h>

class win_hid_ifx: public hid_ifx {

public:
	win_hid_ifx(wchar_t* instance_path);
	~win_hid_ifx();


public:
	// Send a packet to the device through the HID interface
	virtual void send_packet(int8_t* packet, int len) const;

	// Receive a packet from the device through the HID interface
	virtual std::shared_ptr<char> receive_packet() const;

private:
	HANDLE hid_handle;

};