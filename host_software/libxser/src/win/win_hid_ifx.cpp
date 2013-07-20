#include "stdafx.h"
#include "win_hid_ifx.h"

win_hid_ifx::win_hid_ifx(wchar_t* instance_path)
{
}

void win_hid_ifx::send_packet(int8_t* packet, int len) const
{
}


std::shared_ptr<char> win_hid_ifx::receive_packet() const
{
	return 0;
}
