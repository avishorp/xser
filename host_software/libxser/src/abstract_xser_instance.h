#ifndef __ABSTRACT_XSER_INSTANCE__
#define __ABSTRACT_XSER_INSTANCE__

#include <xser.h>
#include <exception>

#define CHECK_VALIDITY \
	if (!is_valid()) throw new std::runtime_error("Invalid Object")

namespace xser {

// This class is a partial implementation of an object representing an xser instance
// in operational (non-DFU) mode. The class implements all the platform-independent
// parts of the object.
// In order to create a complete object, the implementer should provide several
// methods:
//   * get_serial_number - The device serial number read from the USB subsystem
//   * get_hid_io - Returns an object being able to read and write packets to a 
//                  (already opened) HID interface, in platform-independent fashion
//   * get_comm_number - Returns the COM (or tty) number associated with the instance

class EXPORT abstract_xser_instance_oper: public xser::xser_instance_oper_ifx {

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
		CHECK_VALIDITY;
		return false;  
	}

	// Get the serial number associated with the instance
	// Requires platform specific implementation
	virtual const std::string& get_serial_number() const = 0;

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

// This class is a partial implementation of an object representing an xser instance
// in DFU mode. The class implements all the platform-independent
// parts of the object.
// In order to create a complete object, the implementer should provide several
// methods:
//   * get_serial_number - The device serial number read from the USB subsystem
//   * get_hid_io - Returns an object being able to read and write packets to a 
//                  (already opened) HID interface, in platform-independent fashion

class EXPORT abstract_xser_instance_dfu: public xser::xser_instance_dfu_ifx {

protected:
	// Returns an interface to an open HID device, ready to receive and transmit
	// packets
	virtual const hid_ifx& get_hid_io() const = 0;

	// From xser_instance_ifx
	/////////////////////////
public:
	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const {
		// This object represents a device in DFU mode
		CHECK_VALIDITY;
		return true;  
	}

	// Get the serial number associated with the instance
	// Requires platform specific implementation
	virtual const std::string& get_serial_number() const = 0;

	// From xser_instance_dfu_ifx
	//////////////////////////////
public:
	// Program a new firmware
	virtual bool program_firmware(image_t& image, progress_callback_t report_target = NULL) const;

	// Reset the device
	virtual void reset_device() const;

private:
	uint16_t calc_checksum(uint8_t* buf, unsigned int size) const;

	bool wait_for_response_packet() const;
};


// This class is a partial implementation of the xser_instance_manager_ifx
// interface. It implements the update_all_adaptors() method which is independent
// of the specific implementation of the class.
class EXPORT abstract_xser_instance_manager : public xser_instance_manager_ifx {

public:
	virtual void update_all_adaptors();

};

}

#endif // __ABSTRACT_XSER_INSTANCE__
