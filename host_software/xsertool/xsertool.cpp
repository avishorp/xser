#include "xser.h"
#include "win_xser.h"

using namespace xser;

int main(char* argv[], int argc)
{
	xser_instance_manager_ifx& xim = win_xser_instance_manager::get_instance();

	xim.rescan();
}
