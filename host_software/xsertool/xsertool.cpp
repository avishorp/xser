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
		tr1::shared_ptr<const xser_instance_ifx> inst = *iter;
		string version;
		if (inst->is_dfu_mode())
			version = "< DFU >";
		else
			version = "0";

		cout << setw(5) << left << index++ << left << setw(15) << inst->get_serial_number() << version << endl;
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

	shared_ptr<const xser_instance_dfu_ifx> dfu = static_pointer_cast<const xser_instance_dfu_ifx>
		(xsers[program_index-1]);
	int ssss = FIRMWARE_SIZE;
	image_t firmware_image(ihp.get_buffer(), ihp.get_buffer() + FIRMWARE_SIZE);

	dfu->program_firmware(firmware_image, [] (int progress) { cout << "Programming " << progress << "%\r" << flush; });


}
