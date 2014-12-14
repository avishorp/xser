#include <GenericTypeDefs.h>
#include "HardwareProfile.h"
#include "Compiler.h"
#include "autobaud.h"

UINT16 AB_ShortPulseWidth;

void AUTOBAUD_Init()
{
    AUTOBAUD_Disgage();
}

void AUTOBAUD_Engage()
{

}

void AUTOBAUD_Disgage()
{
    AB_ShortPulseWidth = 0xffff;
}

void AUTOBAUD_PinChangeInterruptHandler()
{

}

