#include <xser.h>
#include "../abstract_xser_instance.h"
#include <string>

class win_xser_instance_dfu: public xser::abstract_xser_instance_dfu {

public:
	win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data);
	~win_xser_instance_dfu();


protected:
	virtual const hid_ifx& get_hid_io() const { return *hid_io; }

public:
	virtual const std::string& get_serial_number() const { return serial_number; }

	virtual const uint32_t get_physical_id() const { return 0; }


private:
	std::string serial_number;
	std::auto_ptr<hid_ifx> hid_io;
};