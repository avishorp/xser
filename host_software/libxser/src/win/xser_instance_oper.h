
#ifndef __XSER_INSTANCE_OPER_H__
#define __XSER_INSTANCE_OPER_H__

#include <xser.h>

class xser_instance_oper : public xser::xser_instance_oper_ifx {
public:
	xser_instace_oper(string& _serial_number);

	attach_device(HDEVINFO hdevinfo, PSP_DEVINFO_DATA devinfo_data); 

	/*******************/
	/* xser_device_ifx */
	/*******************/
public:

	// Determine whether the device is in DFU mode
	virtual bool is_dfu_mode() const { return false; };

	// Get the serial number associated with the instance
	virtual std::string& get_serial_number() const { return serial_number; }


	// Find the COM/tty number associated with the device
	virtual int get_associated_com_number() const;

	// Sets the COM number displayed on the LCD
	virtual void set_com_display(int number) const;

	// Retrieve the firmware version number
	virtual void get_firmware_version(int& major, int& minor) const;

	// Enter DFU mode
	virtual void enter_dfu() const;

private:
	hid_ifx* hid;
	string serial_number;
};


#endif // __XSER_INSTANCE_OPER_H__