#include <xser.h>
#include <string>

class win_xser_instance_oper: public xser::abstract_xser_instance_oper {

public:
	win_xser_instance_oper(std::string& serial, HDEVINFO dev_info_set, PSP_DEVINFO_DATA dev_info_data,
		xser::physical_location_t& physical_loc_);
	virtual ~win_xser_instance_oper();


	void process_child(HDEVINFO world_device_info_set, LPCWSTR child_id);

protected:
	virtual hid_ifx& get_hid_io() const;

public:
	virtual const std::string& get_serial_number() const;
public:
	virtual int get_associated_com_number() const;

	virtual const xser::physical_location_t& get_physical_location() const;

	virtual bool is_valid() const { return valid; }

	virtual void enter_dfu();

	virtual void connect();


protected:
	void disconnect();

	void invalidate();


private:
	std::string serial_number;
	int com_number;
	hid_ifx* hid_io;
	bool valid;
	xser::physical_location_t physical_loc;

};
