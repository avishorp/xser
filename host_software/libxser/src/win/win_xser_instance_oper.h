#include <xser.h>
#include <string>

class win_xser_instance_oper: public xser::abstract_xser_instance_oper {

public:
	win_xser_instance_oper(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data);
	~win_xser_instance_oper();


	void process_child(HDEVINFO world_device_info_set, LPCWSTR child_id);

protected:
	virtual const hid_ifx& get_hid_io() const { return *hid_io; }

public:
	virtual const std::string& get_serial_number() const { return serial_number; }

public:
	virtual int get_associated_com_number() const { return com_number; }

	virtual const uint32_t get_physical_id() const { return 0; }

private:
	std::string serial_number;
	int com_number;
	hid_ifx* hid_io;
};
