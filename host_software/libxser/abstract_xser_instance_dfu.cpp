#include <xser.h>
#include <string>
#include <stdint.h>

using namespace std;
using namespace xser;

void abstract_xser_instance_dfu::program_firmware(void* image, int len) const
{
}

void abstract_xser_instance_dfu::finalize_programming() const
{
}
