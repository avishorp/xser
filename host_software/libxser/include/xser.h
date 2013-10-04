// xser.h
//
// Main include file for libxser

#ifndef __XSER_H__
#define __XSER_H__

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <hid_ifx.h>


//////////// Determine build type

#if (defined(_WIN32) || defined(_WIN64)) && defined(_MSC_VER)

#define BUILD_WINDOWS

#else

#if defined(__linux__) && defined(__GNUC__)

#define BUILD_LINUX

#else
#error Only two build types are supported: MSVC under Windows or GCC under Linux
#endif

#endif

////////////////////////////////////


#ifdef BUILD_WINDOWS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// VID & PID of the XSer unit
#define XSER_VID     L"1D50"
#define XSER_PID     L"6059"
#define XSER_PID_DFU L"6069"

// Programming related constants
#define FIRMWARE_ADDR_START    0x1000     // Firmware start address
#define FIRMWARE_ADDR_END      0x3fff     // Firmware end address
#define FIRMWARE_SIZE          (FIRMWARE_ADDR_END - FIRMWARE_ADDR_START + 1)



namespace xser {

// An interface representing an xser device instance connected to the computer.
// This interface is generalized to include devices both in DFU mode and in normal
// operating mode

class EXPORT xser_instance_ifx {
public:

	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const = 0;

	// Get the serial number associated with the instance
	virtual const std::string& get_serial_number() const = 0;
};

typedef EXPORT std::vector <std::tr1::shared_ptr<const xser_instance_ifx>> xser_instances_t;
typedef EXPORT xser_instances_t::const_iterator xser_instances_iter_t;


// An interface representig an xser device instance connected to the computer and
// operating in normal (operational) mode
class EXPORT xser_instance_oper_ifx : public xser_instance_ifx {
public:

	// Find the COM/tty number associated with the device
	virtual int get_associated_com_number() const = 0;

	// Sets the COM number displayed on the LCD
	virtual void set_com_display(int number) const = 0;

	// Retrieve the firmware version number
	virtual void get_firmware_version(int& major, int& minor) const = 0;

	// Enter DFU mode
	virtual void enter_dfu() const = 0;
};

typedef std::vector<unsigned char> image_t;
typedef void (*progress_callback_t)(int);

// An interface representing an xser device instance connected to the computer and
// operating in DFU (firmware upgrade) mode
class EXPORT xser_instance_dfu_ifx: public xser_instance_ifx {
public:
	// Program a new firmware
	virtual bool program_firmware(image_t& image, progress_callback_t report_target = NULL) const = 0;

	// Reset the device
	virtual void reset_device() const = 0;
};

class EXPORT xser_instance_manager_ifx {
public:
	virtual const xser_instances_t& get_xser_instances() const = 0;

	virtual void rescan() = 0;

	virtual void set_verbose_stream(std::ostream&) = 0;

	virtual void update_all_adaptors() = 0;
};

class EXPORT abstract_xser_instance_manager : public xser_instance_manager_ifx {

public:
	virtual void update_all_adaptors();

};

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

#ifdef BUILD_WINDOWS

EXPORT xser_instance_manager_ifx& win_xser_get_instance_manager();

#define get_xser_instance_manager win_xser_get_instance_manager

#else

EXPORT xser_instance_manager_ifx& linux_xser_get_instance_manager();

#define get_xser_instance_manager() linux_xser_get_instance_manager()

#endif

}; // namespace xser


#endif // #ifdef __XSER_H__



