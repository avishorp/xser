// Windows-specific xser classes

#include "xser.h"
#include <vector>
#include <iostream>


class EXPORT win_xser_instance_manager : public xser::xser_instance_manager_ifx {

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
	xser::xser_instances_t xser_instances;
	std::ostream* verbose_stream;
};

