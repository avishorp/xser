// xser.h
//
// Main include file for libxser

#ifndef __XSER_H__
#define __XSER_H__

#include <string>
#include <vector>
#include <memory>
#include <iostream>


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
#define XSER_PID_DFU L"0692"



namespace xser {

// An interface representing an xser device instance connected to the computer.
// This interface is generalized to include devices both in DFU mode and in normal
// operating mode

class EXPORT xser_instance_ifx {
public:

	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const = 0;

	// Get the serial number associated with the instance
	virtual std::string& get_serial_number() const = 0;
};

typedef std::vector <std::tr1::shared_ptr<const xser_instance_ifx>> xser_instances_t;


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

// An interface representing an xser device instance connected to the computer and
// operating in DFU (firmware upgrade) mode
class EXPORT xser_instance_dfu_ifx: public xser_instance_ifx {
public:
	// Program a new firmware
	virtual void program_firmware(void* image, int len) const = 0;

	// Finalize the programming and resume normal operation
	virtual void finalize_programming() const = 0;
};

class EXPORT xser_instance_manager_ifx {
public:
	virtual const xser_instances_t& get_xser_instances() const = 0;

	virtual void rescan() = 0;

	virtual void set_verbose_stream(std::ostream&) = 0;
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



