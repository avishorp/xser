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
		cout << "No XSer adaptors found" << endl;
		exit(0);
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
	bool g = hex_file.good();
	ihp.parse(hex_file);

	xser_instance_ifx* selected = xsers_list[program_index-1];

	if (selected->is_dfu_mode()) {
		const xser_instance_dfu_ifx* dfu = dynamic_cast<const xser_instance_dfu_ifx*>
			(selected);
	
		image_t firmware_image(ihp.get_buffer(), ihp.get_buffer() + FIRMWARE_SIZE);

		if (!(dfu->program_firmware(firmware_image, [] (int progress) { cout << "Programming " << progress << "%\r" << flush; }))) {
			cout << "\nProgramming failed !!!" << endl;
		}
		else {
			cout << "\nProgramming succeedded, resetting device" << endl;
			dfu->reset_device();
		}
	}
	else {
		// The selected xser is not in DFU mode
		xser_instance_oper_ifx* x = dynamic_cast<xser_instance_oper_ifx*>
			(selected);
		x->enter_dfu();
	}


}
