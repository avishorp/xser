// xser.h
//
// Main include file for libxser

#ifndef __XSER_H__
#define __XSER_H__

#include <string>
#include <map>
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

// A type that holds physical location description                       
typedef std::string physical_location_t;

// An interface representing an xser device instance connected to the computer.
// This interface is generalized to include devices both in DFU mode and in normal
// operating mode

class EXPORT xser_instance_ifx {
public:

	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const = 0;

	// Get the serial number associated with the instance
	virtual const std::string& get_serial_number() const = 0;

	// Get the physical location identifier. This identifier need not be
	// in any particular format, the only requirement is that two instances
	// have the same PHYID if and only if they are connected to the same
	// location (USB port) exactly.
	virtual const physical_location_t& get_physical_location() const = 0;

	// Returns true if the object is valid
	virtual bool is_valid() const = 0;

};

typedef EXPORT std::map <physical_location_t, xser_instance_ifx*> xser_instances_t;
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



#ifdef BUILD_WINDOWS

EXPORT xser_instance_manager_ifx& win_xser_get_instance_manager();

#define get_xser_instance_manager win_xser_get_instance_manager

#else

EXPORT xser_instance_manager_ifx& linux_xser_get_instance_manager();

#define get_xser_instance_manager() linux_xser_get_instance_manager()

#endif

}; // namespace xser


#endif // #ifdef __XSER_H__



