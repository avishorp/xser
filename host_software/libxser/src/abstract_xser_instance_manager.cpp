#include <xser.h>
#include "abstract_xser_instance.h"

using namespace xser;

void abstract_xser_instance_manager::update_all_adaptors()
{
	const xser_instances_t& instances = get_xser_instances();
	xser_instances_iter_t i;

	// For each instance, try to set the displayed number to the
	// attached COM port number
	for(i=instances.begin(); i != instances.end(); i++) {
		if (!(i->second->is_dfu_mode())) {
			xser_instance_oper_ifx* ic = dynamic_cast<xser_instance_oper_ifx*>(i->second);
			ic->set_com_display(ic->get_associated_com_number());
		}
	}
}
