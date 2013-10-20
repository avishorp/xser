#include "xser.h"
#include "ihex_parser.h"
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <functional>

using namespace xser;
using namespace std;
using namespace ihex_parser_ns;
using namespace tr1;

// Default firmware file
const char default_firmware[] = "..\\..\\firmware\\xser\\dist\\default\\production\\xser.production.hex";

// Forwards
xser_instance_dfu_ifx* switch_to_dfu(xser_instance_oper_ifx* inst);
void do_programming(xser_instance_ifx* inst, image_t& firmware_image);


int main(char* argv[], int argc)
{
	xser_instance_manager_ifx& xim = get_xser_instance_manager();

	xim.set_verbose_stream(cout);
	xim.rescan();
	//xim.update_all_adaptors();

	const xser_instances_t& xsers = xim.get_xser_instances();
	vector<xser_instance_ifx*> xsers_list;
	int count = xsers.size();

	if (count == 0) {
		cerr << "No XSer adaptors found" << endl;
		exit(1);
	}

	cout << "Found " << count << " xser adaptors" << endl;

	cout << setw(5) << left << "No." << setw(15) << left << "Serial No." << "Version" << endl;
	cout << setw(5) << left << "---" << setw(15) << left << "----------" << left << "-------" << endl;
	xser_instances_iter_t iter = xsers.begin();
	int index = 1;

	for(;iter != xsers.end(); iter++) {
		xser_instance_ifx* inst = iter->second;
		string version;
		if (inst->is_dfu_mode())
			version = "< DFU >";
		else
			version = "0";

		cout << setw(5) << left << index++ << left << setw(15) << inst->get_serial_number() << version << endl;

		xsers_list.push_back(inst);
	}

	bool program_all = false;
	int program_index;
	char response[250];
	memset(response, 0, sizeof(response));
	while(1) {
		cout << "Select the adaptor number to be programmed or 'all' [all]  ";
		cin.getline(response, 10);

		if ((strcmp(response, "all" ) == 0) || (strlen(response) == 0)) {
			program_all = true;
			break;
		}

		program_index = atoi(response);
		if ((program_index > 0) && (program_index <= count)) {
			break;
		}

	}

	string firmware_file;
	memset(response, 0, sizeof(response));
	cout << "HEX file of the firmware [" << default_firmware << "]" << endl;
	cin.getline(response, sizeof(response)-1);

	if (strlen(response) == 0) 
		firmware_file = default_firmware;
	else
		firmware_file = response;

	cout << "Reading HEX file " << firmware_file << endl; 
	ihex_parser ihp(FIRMWARE_ADDR_START, FIRMWARE_ADDR_END);
	ifstream hex_file;
	hex_file.open(firmware_file, ios::in);
	if (!hex_file.good()) {
		cerr << "Cannot open hex file: " << strerror( errno ) << endl;
		exit(1);
	}
	ihp.parse(hex_file);
	image_t firmware_image(ihp.get_buffer(), ihp.get_buffer() + FIRMWARE_SIZE);

	if (program_all) {
		for (xser_instances_iter_t it = xsers.begin(); it != xsers.end(); it++)
			do_programming(it->second, firmware_image);
	}
	else {
		xser_instance_ifx* selected = xsers_list[program_index-1];
		do_programming(selected, firmware_image);
	}


}

xser_instance_dfu_ifx* switch_to_dfu(xser_instance_oper_ifx* inst)
{
	cout << "Device " << inst->get_serial_number() << " is not in DFU mode, switching" << endl;
	// Get the instance manager
	xser_instance_manager_ifx& inst_manager = get_xser_instance_manager();

	// Get the physical location of the instance
	physical_location_t loc = inst->get_physical_location();

	// Give the command to switch to DFU mode
	inst->enter_dfu();

	// Wait until the adapter is switched
	for(int iter = 20; iter >= 0; iter--) {
		cout << "Wait for the switch to complete ... " << endl;
		_sleep(1000);

		try {
			inst_manager.rescan();
			const xser_instances_t& xser_instances = inst_manager.get_xser_instances();

			xser_instances_iter_t t = xser_instances.find(loc);
			if (t != xser_instances.end()) {
				if (t->second->is_dfu_mode())
					return dynamic_cast<xser_instance_dfu_ifx*>(t->second);
			}
		}
		catch (runtime_error& e) {
			// During transitions, errors may occur, we will ignore then
		}
	}

	// Something went wrong ...
	return NULL;
}

void do_programming(xser_instance_ifx* inst, image_t& firmware_image)
{
	xser_instance_dfu_ifx* dfu_inst;

	// Check if the instance is in DFU mode, and switch it
	// to that mode if not
	if (inst->is_dfu_mode()) {
		// Already in DFU mode
		dfu_inst = dynamic_cast<xser_instance_dfu_ifx*>(inst);
	}
	else {
		// Not in DFU mode
		dfu_inst = switch_to_dfu(dynamic_cast<xser_instance_oper_ifx*>(inst));
		if (dfu_inst == NULL) {
			cerr << "Failed switching the device into DFU mode" << endl;
			exit(1);
		}
	}

	if (!(dfu_inst->program_firmware(firmware_image, [] (int progress) { cout << "Programming " << progress << "%\r" << flush; }))) {
			cout << "\nProgramming failed !!!" << endl;
	}
	else {
		cout << "\nProgramming succeedded, resetting device" << endl;
		dfu_inst->reset_device();
	}

}


