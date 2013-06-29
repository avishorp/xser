// xser.h
//
// Main include file for libxser

#include <string>
#include <vector>

// An interface representing an xser device instance connected to the computer.
// This interface is generalized to include devices both in DFU mode and in normal
// operating mode

class xser_instance_ifx {
public:

	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const = 0;

	// Get the serial number associated with the instance
	virtual std::string& get_serial_number() const = 0;
};



// An interface representig an xser device instance connected to the computer and
// operating in normal (operational) mode
class xser_instance_oper_ifx : public xser_instance_ifx {
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
class xser_instance_dfu_ifx: public xser_instance_ifx {
public:
	// Program a new firmware
	virtual void program_firmware(void* image, int len) const = 0;

	// Finalize the programming and resume normal operation
	virtual void finalize_programming() const = 0;
};

class xser_instance_manager_ifx {
public:
	virtual const std::vector <const xser_instance_ifx&> get_xser_instances() const;

	virtual void rescan();
};



