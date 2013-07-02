// libxser.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "win_xser.h"
#include <memory>

using namespace std;
using namespace xser;


// Default value for the instance variable
xser_instance_manager_ifx* win_xser_instance_manager::instance = 0;

xser_instance_manager_ifx& win_xser_instance_manager::get_instance()
{
	if (!instance)
		instance = new win_xser_instance_manager();

	return *instance;
}

win_xser_instance_manager::win_xser_instance_manager() : xser_instances()
{
}

const xser_instances_t& win_xser_instance_manager::get_xser_instances() const 
{
	return xser_instances;
}

void win_xser_instance_manager::rescan()
{
	vector<shared_ptr<int>> z;

	// First, clean the list of current instances
	while(xser_instances.size() > 0) {
		std::tr1::shared_ptr<const xser_instance_ifx> xs = xser_instances.back();
		xser_instances.pop_back();
		xs.reset();
	}

	// Scan all the USB devices connected to the computer
}

