#include <xser.h>
#include <string>

class win_xser_instance_dfu: public xser::abstract_xser_instance_dfu {

public:
	win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data);
	~win_xser_instance_dfu();


protected:
	virtual const hid_ifx& get_hid_io() const { return *hid_io; }

public:
	virtual const std::string& get_serial_number() const { return serial_number; }

private:
	std::string serial_number;
	hid_ifx* hid_io;
};
