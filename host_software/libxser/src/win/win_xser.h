// Windows-specific xser classes

#include "xser.h"
#include <vector>
#include <iostream>
#include <hid_ifx.h>


class EXPORT win_xser_instance_manager : public xser::abstract_xser_instance_manager {

public:
	// Get the Singleton instance
	static xser::xser_instance_manager_ifx& get_instance();

protected:
	// Protected constructor to prevent construction
	win_xser_instance_manager();

private:
	static xser::xser_instance_manager_ifx* instance;

	// Implementation of xser_instance_manager_ifx
public:
	virtual const xser::xser_instances_t& get_xser_instances() const;

	virtual void rescan();

protected:
	virtual std::ostream& get_verbose_stream() { return *verbose_stream; }

public:
	virtual void set_verbose_stream(std::ostream& vs) { verbose_stream = &vs; }

private:
	std::auto_ptr<std::string> get_serial_number(HDEVINFO device_info_set, PSP_DEVINFO_DATA device_info_data);

	xser::xser_instances_t xser_instances;
	std::ostream* verbose_stream;
};


