//
// Description: An implementation of xser_instance_oper. The implementation is platform-independent
//              and relies on platform-dependent functions for HID read & write and for the association
//              of an XSer device and a COM port

#ifndef  __ABSTRACT_XSER_INSTANCE_OPER_H__
#define  __ABSTRACT_XSER_INSTANCE_OPER_H__

#include <xser.h>
#include <hid_ifx.h>

class abstract_xser_instance_oper: public xser::xser_instance_oper_ifx {

protected:
	// Returns an interface to an open HID device, ready to receive and transmit
	// packets
	virtual const hid_ifx& get_hid_io() const = 0;

	// From xser_instance_ifx
	/////////////////////////
public:
	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const {
		// This object represents a device in operational (non-DFU) mode
		return false;  
	}

	// Get the serial number associated with the instance
	// Requires platform specific implementation
	virtual std::string& get_serial_number() const = 0;

	// From xser_instance_oper_ifx
	//////////////////////////////
public:
	// Find the COM/tty number associated with the device
	// Requires platform-specific implementation
	virtual int get_associated_com_number() const = 0;

	// Sets the COM number displayed on the LCD
	virtual void set_com_display(int number) const;

	// Retrieve the firmware version number
	virtual void get_firmware_version(int& major, int& minor) const;

	// Enter DFU mode
	virtual void enter_dfu() const;

};

#endif // ! __ABSTRACT_XSER_INSTANCE_OPER_H__
