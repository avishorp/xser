#include "xser.h"

using namespace xser;
using namespace std;


int main(char* argv[], int argc)
{
	xser_instance_manager_ifx& xim = get_xser_instance_manager();

	xim.set_verbose_stream(cout);
	xim.rescan();
}
