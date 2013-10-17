#include <xser.h>

class win_hid_ifx: public hid_ifx {

public:
	win_hid_ifx(wchar_t* instance_path);
	~win_hid_ifx();

public:
	// A factory method for creating a HID interface object from a child node of a USB device
	static std::auto_ptr<hid_ifx> from_child(LPCWSTR instance_id);

	// Send a packet to the device through the HID interface
	virtual void send_packet(uint8_t* packet, int len) const;

	// Receive a packet from the device through the HID interface
	virtual std::shared_ptr<char> receive_packet() const;

	virtual void set_timeout(int to) const;

	void open();

	void close();

private:
	HANDLE hid_handle;
	std::wstring instance_path;

};