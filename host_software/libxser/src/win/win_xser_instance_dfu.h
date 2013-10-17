#include <xser.h>
#include "../abstract_xser_instance.h"
#include <string>

class win_xser_instance_dfu: public xser::abstract_xser_instance_dfu {

public:
	win_xser_instance_dfu(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data,
		xser::physical_location_t& physical_loc_);
	~win_xser_instance_dfu();


protected:
	virtual hid_ifx& get_hid_io() const;

public:
	virtual const std::string& get_serial_number() const;

	virtual const xser::physical_location_t& get_physical_location() const;

	virtual bool is_valid() const { return valid; }

	virtual void reset_device();

	virtual void connect();

protected:

	void disconnect();

	void invalidate();



private:
	std::string serial_number;
	std::auto_ptr<hid_ifx> hid_io;
	bool valid;
	xser::physical_location_t physical_loc;
};
