// This code replaces the bootloader, in order to ease devolopment
// and allow programming using PicKit 3
// When using the bootloader to upload a new firmware, it is ignored
// since it's not in the programmable address range

#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
    _asm goto 0x1008 _endasm
}
#pragma code low_vector=0x18
void interrupt_at_low_vector(void)
{
    _asm goto 0x1018 _endasm
}



