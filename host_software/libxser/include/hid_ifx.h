//
// Description: Interface for generic, platform-independent HID I/O
//

#ifndef __HID_IFX_H__
#define __HID_IFX_H__

#include <memory>

class hid_ifx {

public:
	// Send a packet to the device through the HID interface
	virtual void send_packet(uint8_t* packet, int len) const = 0;

	// Receive a packet from the device through the HID interface
	virtual std::shared_ptr<char> receive_packet() const = 0;
};

#endif // #ifndef __HID_IFX_H__
