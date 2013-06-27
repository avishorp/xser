
#include "USB/usb.h"
#include "USB/usb_function_cdc.h"
#include "USB/usb_function_hid.h"
#include "HardwareProfile.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "usb_config.h"
#include "USB/usb_device.h"
#include "USB/usb.h"
#include "HardwareProfile.h"
#include "ui.h"
#include "usart.h"


#pragma udata

// CDC
char CDC_OutData[CDC_DATA_OUT_EP_SIZE]; // Host-to-TX
char CDC_InData[CDC_DATA_IN_EP_SIZE];   // RX-to-Host
unsigned char CDC_OutDataPointer;
unsigned char CDC_OutDataCount;
unsigned char CDC_InDataPointer;
unsigned char CDC_InDataCount;

// HID
USB_HANDLE HID_OutHandle = 0;    //USB handle.  Must be initialized to 0 at startup.
USB_HANDLE HID_InHandle = 0;     //USB handle.  Must be initialized to 0 at startup.

#pragma udata USB_VARS
unsigned char HID_OutDataBuffer[64]; // Host-to-xser
//unsigned char HID_InDataBuffer[64] TX_DATA_BUFFER_ADDRESS; // xser-to-Host
#pragma udata


// Prototypes
/////////////
void ProcessIO(void);
void SystemInit();
void USB_SetSerialNumber();
void CDC_Init();
unsigned char CDC_Service();
void HID_Init();
unsigned char HID_Service();
void USBCBSendResume(void);
void UserInit(void);
void InitializeUSART(void);
void putcUSART(char c);
unsigned char getcUSART ();


// TODO: Move these to a separate h file
#define REMAPPED_RESET_VECTOR_ADDRESS		0x00
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS	0x08
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS	0x18
	
#pragma code
	

void main(void)
{
    char is_configured = 0;
    long k;
    unsigned char events = 0;

    SystemInit();

    // Clear HID in and out handles
    HID_OutHandle = 0;
    HID_InHandle = 0;


    // Initialize the USB
    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware

    while(1)
    {
        events = 0;
        
	// Check bus status and service USB interrupts.
        USBDeviceTasks();    				  

        if (is_configured) {
            // Device is already configured
            events |= CDC_Service();
            events |= HID_Service();
        }
        else {
            // Device is not configured

            if (USBDeviceState == CONFIGURED_STATE) {
                is_configured = 1;
                events = EVENT_USBCONF;
            }
        }

        UI_Service(events);
    }//end while
}//end main



void SystemInit()
{
    // Set the oscillator to 16MHz
    OSCCON = 0b01110000;

    // Initialize the I/O pins
    IO_Init();
    ANSELCbits.ANSC7 = 0; // Make the RX pin digital
    ANSELCbits.ANSC2 = 0; // Make the DSR pin digital

    // Initialize the UI module & Enable its interrupts
    // (required for LCD operation)
    UI_Init();
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;

    // Initialize the USART
    USART_Init();
    USART_SetBaud(600);
    
    // Read the device's serial number from the EEPROM
    // and set it as the USB seroal number
    USB_SetSerialNumber();

    // Initialize the CDC handler
    CDC_Init();
    // Initialize the HID handler
    HID_Init();

}


// Read the serial number from the EEPROM
void USB_SetSerialNumber()
{
    char i;

    // A Pointer to the ID location
    rom char far *pID=(rom char far *)0x200000;

    // Initialize the string descriptor
    sdSerial.bLength = sizeof(sdSerial);
    sdSerial.bDscType = USB_DESCRIPTOR_STRING;

    // Temporarily disable interrupts
    INTCONbits.GIE=0;

    for(i=0; i<8; i++)
    {
        // Read the EEPROM data into the serial number
        // variable
        sdSerial.string[i] = '0' + (*pID++);

    }

    // Reenable interrupts
    TBLPTRU=0;            // sect. 3.1 C18 User's guide...
    INTCONbits.GIE=1;
}


void CDC_Init()
{
    CDC_OutDataCount = 0;
    CDC_OutDataPointer = 0;
    CDC_InDataCount = 0;
    CDC_InDataPointer = 0;

}

// Perform all CDC related tasks
// Returns activity indication - bit 0 indicates RX activity, bit 1 TX activity
unsigned char CDC_Service()
{
    unsigned char activity = 0;

    // If the device is not configured yet, do nothing
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) return 0;

    // Host-to-Serial
    /////////////////
    if (CDC_OutDataCount > 0) {
        // Data is waiting to be transmitted on the serial
        // port (from previous USB transaction)
        if (USART_IsTxEmpty()) {
            // The serial port is ready for transmission
            USART_SendByte(CDC_OutData[CDC_OutDataPointer]);
            CDC_OutDataPointer++;
            CDC_OutDataCount--;
            activity = 0x01;
        }
    }
    else {
        // No data waiting to be transmitted - check if there is
        // data received from the USB
        CDC_OutDataCount = getsUSBUSART(CDC_OutData, 64);
        CDC_OutDataPointer = 0;
    }

    // Serial-to-Host
    /////////////////
    if (USART_IsRxAvail()) {
        // A character has been received in the
        // serial port
        if (CDC_InDataCount < CDC_DATA_IN_EP_SIZE) {
            // Enough space available in the input
            // buffer
            CDC_InData[CDC_InDataPointer] = USART_GetByte();
            CDC_InDataPointer++;
            CDC_InDataCount++;
            activity |= 0x02;
        }
    }

    if (USBUSARTIsTxTrfReady())
    {
        if (CDC_InDataCount > 0) {
            // Data ready to be sent
            putUSBUSART(&CDC_InData[0], CDC_InDataCount);
            CDC_InDataCount = 0;
            CDC_InDataPointer = 0;
        }
    }

    CDCTxService();

    return activity;

/*




    }
	if (RS232_Out_Data_Rdy == 0)  // only check for new USB buffer if the old RS232 buffer is
	{						  // empty.  This will cause additional USB packets to be NAK'd
		LastRS232Out = getsUSBUSART(RS232_Out_Data,64); //until the buffer is free.
		if(LastRS232Out > 0)
		{	
			RS232_Out_Data_Rdy = 1;  // signal buffer full
			RS232cp = 0;  // Reset the current position
		}
	}

    //Check if one or more bytes are waiting in the physical UART transmit
    //queue.  If so, send it out the UART TX pin.
	if(RS232_Out_Data_Rdy && mTxRdyUSART())
	{
    	#if defined(USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL)
        	//Make sure the receiving UART device is ready to receive data before 
        	//actually sending it.
        	if(UART_CTS == USB_CDC_CTS_ACTIVE_LEVEL)
        	{
        		putcUSART(RS232_Out_Data[RS232cp]);
        		++RS232cp;
        		if (RS232cp == LastRS232Out)
        			RS232_Out_Data_Rdy = 0;
    	    }
	    #else
	        //Hardware flow control not being used.  Just send the data.
    		putcUSART(RS232_Out_Data[RS232cp]);
    		++RS232cp;
    		if (RS232cp == LastRS232Out)
    			RS232_Out_Data_Rdy = 0;	    
	    #endif
	}

    //Check if we received a character over the physical UART, and we need
    //to buffer it up for eventual transmission to the USB host.
	if(mDataRdyUSART() && (NextUSBOut < (CDC_DATA_OUT_EP_SIZE - 1)))
	{
		USB_Out_Buffer[NextUSBOut] = getcUSART();
		++NextUSBOut;
		USB_Out_Buffer[NextUSBOut] = 0;
	}
	
	#if defined(USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL)
    	//Drive RTS pin, to let UART device attached know if it is allowed to 
    	//send more data or not.  If the receive buffer is almost full, we 
    	//deassert RTS.
    	if(NextUSBOut <= (CDC_DATA_OUT_EP_SIZE - 5u))
    	{
            UART_RTS = USB_CDC_RTS_ACTIVE_LEVEL;
        }   	
        else
        {
        	UART_RTS = (USB_CDC_RTS_ACTIVE_LEVEL ^ 1);
        }    
    #endif	

    //Check if any bytes are waiting in the queue to send to the USB host.
    //If any bytes are waiting, and the endpoint is available, prepare to
    //send the USB packet to the host.
	if((USBUSARTIsTxTrfReady()) && (NextUSBOut > 0))
	{
	  putUSBUSART(&USB_Out_Buffer[0], NextUSBOut);
	  NextUSBOut = 0;
	}

    CDCTxService();

    // Handle HID packets
    if(!HIDRxHandleBusy(HIDOutHandle))
 
    {
        // Display the LSB digit
//        digit = ReceivedDataBuffer[0] % 10;
        // (in the prototype, we only have a single digit)

        // Re-arm the HID EP
        HIDOutHandle = HIDRxPacket(HID_EP, (BYTE*)&ReceivedDataBuffer, 64);
    }
*/

}//end ProcessIO

// Handle SetLineCoding requests
void CDC_SetLineCodingHandler(void)
{
    DWORD_VAL dwBaud;

    //If the request is not in a valid range
    if(cdc_notice.GetLineCoding.dwDTERate.Val <= MAX_BAUD_RATE)
    {
        //Update the baudrate info in the CDC driver
        CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate.Val);

        // Set the actual rate
        USART_SetBaud(cdc_notice.GetLineCoding.dwDTERate.Val);
    }
}

// Global HID Handler initialization
void HID_Init()
{
    HID_InHandle = 0;
    HID_OutHandle = 0;
}

// Initialize the HID endpoint (called after configuration)
void HID_EP_Init()
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);

    //Re-arm the OUT endpoint for the next packet
    HID_OutHandle = HIDRxPacket(HID_EP,(BYTE*)&HID_OutDataBuffer,64);
}

unsigned char HID_Service()
{
    unsigned char event = 0;

    // Handle HID packets
    if(!HIDRxHandleBusy(HID_OutHandle))

    {
        // Display the LSB digit
        UI_SetPortNumber(HID_OutDataBuffer[0]);
        event = EVENT_HIDCMD;

        // Re-arm the HID EP
        HID_OutHandle = HIDRxPacket(HID_EP, (BYTE*)&HID_OutDataBuffer, 64);
    }

    return event;
}

// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA* each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

// Note *: The "usb_20.pdf" specs indicate 500uA or 2.5mA, depending upon device classification. However,
// the USB-IF has officially issued an ECN (engineering change notice) changing this to 2.5mA for all 
// devices.  Make sure to re-download the latest specifications to get all of the newest ECNs.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
	//Example power saving code.  Insert appropriate code here for the desired
	//application behavior.  If the microcontroller will be put to sleep, a
	//process similar to that shown below may be used:
	
	//ConfigureIOPinsForLowPower();
	//SaveStateOfAllInterruptEnableBits();
	//DisableAllInterruptEnableBits();
	//EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
	//Sleep();
	//RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.
	//RestoreIOPinsToNormal();									//Preferrably, this should be done in the USBCBWakeFromSuspend() function instead.

	//IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is 
	//cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause 
	//things to not work as intended.	
	

 
}



/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signalling.
 *					
 *					This call back is invoked when a wakeup from USB suspend 
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
	// If clock switching or other power savings measures were taken when
	// executing the USBCBSuspend() function, now would be a good time to
	// switch back to normal full power run mode conditions.  The host allows
	// 10+ milliseconds of wakeup time, after which the device must be 
	// fully back to normal, and capable of receiving and processing USB
	// packets.  In order to do this, the USB module must receive proper
	// clocking (IE: 48MHz clock must be available to SIE for full speed USB
	// operation).  
	// Make sure the selected oscillator settings are consistent with USB 
    // operation before returning from this function.
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

	// Typically, user firmware does not need to do anything special
	// if a USB error occurs.  For example, if the host sends an OUT
	// packet to your device, but the packet gets corrupted (ex:
	// because of a bad connection, or the user unplugs the
	// USB cable during the transmission) this will typically set
	// one or more USB error interrupt flags.  Nothing specific
	// needs to be done however, since the SIE will automatically
	// send a "NAK" packet to the host.  In response to this, the
	// host will normally retry to send the packet again, and no
	// data loss occurs.  The system will typically recover
	// automatically, without the need for application firmware
	// intervention.
	
	// Nevertheless, this callback function is provided, such as
	// for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and 
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific 
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckCDCRequest();
    USBCheckHIDRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This 
 *					callback function should initialize the endpoints 
 *					for the device's usage according to the current 
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //Enable the CDC data endpoints
    CDCInitEP();
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *					
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes 
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function will only be able to wake up the host if
 *                  all of the below are true:
 *					
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET 
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.   
 *
 *                  If the host has not armed the device to perform remote wakeup,
 *                  then this function will return without actually performing a
 *                  remote wakeup sequence.  This is the required behavior, 
 *                  as a USB device that has not been armed to perform remote 
 *                  wakeup must not drive remote wakeup signalling onto the bus;
 *                  doing so will cause USB compliance testing failure.
 *                  
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            This function does nothing and returns quickly, if the USB
 *                  bus and host are not in a suspended condition, or are 
 *                  otherwise not in a remote wakeup ready state.  Therefore, it
 *                  is safe to optionally call this function regularly, ex: 
 *                  anytime application stimulus occurs, as the function will
 *                  have no effect, until the bus really is in a state ready
 *                  to accept remote wakeup. 
 *
 *                  When this function executes, it may perform clock switching,
 *                  depending upon the application specific code in 
 *                  USBCBWakeFromSuspend().  This is needed, since the USB
 *                  bus will no longer be suspended by the time this function
 *                  returns.  Therefore, the USB module will need to be ready
 *                  to receive traffic from the host.
 *
 *                  The modifiable section in this routine may be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of ~3-15 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;
    
    //First verify that the host has armed us to perform remote wakeup.
    //It does this by sending a SET_FEATURE request to enable remote wakeup,
    //usually just before the host goes to standby mode (note: it will only
    //send this SET_FEATURE request if the configuration descriptor declares
    //the device as remote wakeup capable, AND, if the feature is enabled
    //on the host (ex: on Windows based hosts, in the device manager 
    //properties page for the USB device, power management tab, the 
    //"Allow this device to bring the computer out of standby." checkbox 
    //should be checked).
    if(USBGetRemoteWakeupStatus() == TRUE) 
    {
        //Verify that the USB bus is in fact suspended, before we send
        //remote wakeup signalling.
        if(USBIsBusSuspended() == TRUE)
        {
            USBMaskInterrupts();
            
            //Clock switch to settings consistent with normal USB operation.
            USBCBWakeFromSuspend();
            USBSuspendControl = 0; 
            USBBusIsSuspended = FALSE;  //So we don't execute this code again, 
                                        //until a new suspend condition is detected.

            //Section 7.1.7.7 of the USB 2.0 specifications indicates a USB
            //device must continuously see 5ms+ of idle on the bus, before it sends
            //remote wakeup signalling.  One way to be certain that this parameter
            //gets met, is to add a 2ms+ blocking delay here (2ms plus at 
            //least 3ms from bus idle to USBIsBusSuspended() == TRUE, yeilds
            //5ms+ total delay since start of idle).
            delay_count = 3600U;        
            do
            {
                delay_count--;
            }while(delay_count);
            
            //Now drive the resume K-state signalling onto the USB bus.
            USBResumeControl = 1;       // Start RESUME signaling
            delay_count = 1800U;        // Set RESUME line for 1-13 ms
            do
            {
                delay_count--;
            }while(delay_count);
            USBResumeControl = 0;       //Finished driving resume signalling

            USBUnmaskInterrupts();
        }
    }
}


/*******************************************************************
 * Function:        void USBCBEP0DataReceived(void)
 *
 * PreCondition:    ENABLE_EP0_DATA_RECEIVED_CALLBACK must be
 *                  defined already (in usb_config.h)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called whenever a EP0 data
 *                  packet is received.  This gives the user (and
 *                  thus the various class examples a way to get
 *                  data that is received via the control endpoint.
 *                  This function needs to be used in conjunction
 *                  with the USBCBCheckOtherReq() function since 
 *                  the USBCBCheckOtherReq() function is the apps
 *                  method for getting the initial control transfer
 *                  before the data arrives.
 *
 * Note:            None
 *******************************************************************/
#if defined(ENABLE_EP0_DATA_RECEIVED_CALLBACK)
void USBCBEP0DataReceived(void)
{
}
#endif

/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(int event, void *pdata, WORD size)
{
    switch(event)
    {
        case EVENT_TRANSFER:
            //Add application specific callback task or callback function here if desired.
            break;
        case EVENT_SOF:
            USBCB_SOF_Handler();
            break;
        case EVENT_SUSPEND:
            USBCBSuspend();
            break;
        case EVENT_RESUME:
            USBCBWakeFromSuspend();
            break;
        case EVENT_CONFIGURED: 
            USBCBInitEP();
            HID_EP_Init();
            break;
        case EVENT_SET_DESCRIPTOR:
            USBCBStdSetDscHandler();
            break;
        case EVENT_EP0_REQUEST:
            USBCBCheckOtherReq();
            break;
        case EVENT_BUS_ERROR:
            USBCBErrorHandler();
            break;
        case EVENT_TRANSFER_TERMINATED:
            //Add application specific callback task or callback function here if desired.
            //The EVENT_TRANSFER_TERMINATED event occurs when the host performs a CLEAR
            //FEATURE (endpoint halt) request on an application endpoint which was 
            //previously armed (UOWN was = 1).  Here would be a good place to:
            //1.  Determine which endpoint the transaction that just got terminated was 
            //      on, by checking the handle value in the *pdata.
            //2.  Re-arm the endpoint if desired (typically would be the case for OUT 
            //      endpoints).
            break;
        default:
            break;
    }      
    return TRUE; 
}

/** EOF main.c *************************************************/
