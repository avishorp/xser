
#ifndef USBCFG_H
#define USBCFG_H

// USB VID & PID generously provided
// by OpenMoko project. See http://wiki.openmoko.org/wiki/USB_Product_IDs
#define USB_VID        0x1d50
#define USB_PID_FULL   0x6059
#define USB_PID_LEGACY 0x0000 // TBD

#define USB_EP0_BUFF_SIZE		8	// Valid Options: 8, 16, 32, or 64 bytes.
								// Using larger options take more SRAM, but
									
#define USB_MAX_NUM_INT     	2   // For tracking Alternate Setting

// Configure ping-pong mode
#define USB_PING_PONG_MODE USB_PING_PONG__FULL_PING_PONG

// Software uses main loop polling method
#define USB_POLLING

// Enable internal pull-ups
#define USB_PULLUP_OPTION USB_PULLUP_ENABLE

// Use internal transciever
#define USB_TRANSCEIVER_OPTION USB_INTERNAL_TRANSCEIVER

// Full speed device
#define USB_SPEED_OPTION USB_FULL_SPEED
//#define USB_SPEED_OPTION USB_LOW_SPEED //(not valid option for PIC24F devices)

// TODO: Make sure it's OK to disable this
//#define USB_ENABLE_STATUS_STAGE_TIMEOUTS    //Comment this out to disable this feature.

//Section 9.2.6 of the USB 2.0 specifications indicate that:
//1.  Control transfers with no data stage: Status stage must complete within 
//      50ms of the start of the control transfer.
//2.  Control transfers with (IN) data stage: Status stage must complete within 
//      50ms of sending the last IN data packet in fullfilment of the data stage.
//3.  Control transfers with (OUT) data stage: No specific status stage timing
//      requirement.  However, the total time of the entire control transfer (ex:
//      including the OUT data stage and IN status stage) must not exceed 5 seconds.
//
//Therefore, if the USB_ENABLE_STATUS_STAGE_TIMEOUTS feature is used, it is suggested
//to set the USB_STATUS_STAGE_TIMEOUT value to timeout in less than 50ms.  If the
//USB_ENABLE_STATUS_STAGE_TIMEOUTS feature is not enabled, then the USB_STATUS_STAGE_TIMEOUT
//parameter is not relevant.

#define USB_STATUS_STAGE_TIMEOUT     (BYTE)45   //Approximate timeout in milliseconds, except when
                                                //USB_POLLING mode is used, and USBDeviceTasks() is called at < 1kHz
                                                //In this special case, the timeout becomes approximately:
//Timeout(in milliseconds) = ((1000 * (USB_STATUS_STAGE_TIMEOUT - 1)) / (USBDeviceTasks() polling frequency in Hz))
//------------------------------------------------------------------------------------------------------------------

#define USB_SUPPORT_DEVICE

#define USB_NUM_STRING_DESCRIPTORS 4

// Serial number string descriptor
//////////////////////////////////

// The string descriptor used for the serial number
#define USB_SERIAL_STRING_DESCRIPTOR 3
// The variable name from which the serial number
// descriptor will be taken
#define USB_SERIAL_STRING_VANAME     sdSerial

// TODO: Check if anything here is needed
//#define USB_INTERRUPT_LEGACY_CALLBACKS
#define USB_ENABLE_ALL_HANDLERS
//#define USB_ENABLE_SUSPEND_HANDLER
//#define USB_ENABLE_WAKEUP_FROM_SUSPEND_HANDLER
//#define USB_ENABLE_SOF_HANDLER
//#define USB_ENABLE_ERROR_HANDLER
//#define USB_ENABLE_OTHER_REQUEST_HANDLER
//#define USB_ENABLE_SET_DESCRIPTOR_HANDLER
//#define USB_ENABLE_INIT_EP_HANDLER
//#define USB_ENABLE_EP0_DATA_HANDLER
//#define USB_ENABLE_TRANSFER_COMPLETE_HANDLER

// Device classes to use
#define USB_USE_CDC
#define USB_USE_HID

// Endpoints
////////////

#define USB_MAX_EP_NUMBER	    3

// HID - One endpoint (1)
#define HID_INTF_ID             0
#define HID_EP 			1
#define HID_INT_OUT_EP_SIZE     3
#define HID_INT_IN_EP_SIZE      3
#define HID_NUM_OF_DSC          1
#define HID_RPT01_SIZE          28

// CDC - Two endpoints: control endpoint (2) and data endpoint (3)
#define CDC_COMM_INTF_ID        1
#define CDC_COMM_EP             2
#define CDC_COMM_IN_EP_SIZE     10

#define CDC_DATA_INTF_ID        2
#define CDC_DATA_EP             3
#define CDC_DATA_OUT_EP_SIZE    64
#define CDC_DATA_IN_EP_SIZE     64

// TODO: Replace this awful name
#define USB_CDC_SET_LINE_CODING_HANDLER CDC_SetLineCodingHandler


//------------------------------------------------------------------------------                                                
//Uncomment the "extra UART signals" that are desired to be enabled/supported.  
//These items are optional and none of them are required for basic RX and TX
//USB to UART translator devices.
//If one or more of the below options is enabled however, make sure that the
//polarity is selected correctly, and that the respective pin definitions 
//(ex: UART_RTS) and initialization macros (ex: mInitRTSPin()) are present and 
//defined correctly in the HardwareProfile - [platform name].h file.  
//------------------------------------------------------------------------------                                                
#define USB_CDC_SUPPORT_DSR_REPORTING   //Signal from UART peripheral device, to CDC/USB host.  Indicates UART peripheral is ready to receive data and/or commands.
#define USB_CDC_SUPPORT_DTR_SIGNALING   //Signal sent from the USB/CDC host, down to the UART peripheral device
//#define USB_CDC_SUPPORT_HARDWARE_FLOW_CONTROL   //Implements RTS/CTS UART flow control.

//RTS is GPIO output signal from CDC/USB host micro (indicates UART Rx buffer 
//   space available, and that the UART peripheral device is free to send data to the USB CDC micro)
//CTS is GPIO input on CDC/USB host micro (allows UART peripheral to tell the 
//   UART host [the USB micro] not to send anymore Tx data to it for awhile)

//Define the logic level for the "active" state.  Setting is only relevant if 
//the respective function is enabled.  Allowed options are:
//1 = active state logic level is Vdd
//0 = active state logic level is Vss
#define USB_CDC_CTS_ACTIVE_LEVEL    0
#define USB_CDC_RTS_ACTIVE_LEVEL    1
#define USB_CDC_DSR_ACTIVE_LEVEL    1
#define USB_CDC_DTR_ACTIVE_LEVEL    0                                      

//#define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D2 //Send_Break command
#define USB_CDC_SUPPORT_ABSTRACT_CONTROL_MANAGEMENT_CAPABILITIES_D1 //Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and Serial_State commands

// Serial number string descriptor
typedef struct {
    BYTE bLength;
    BYTE bDscType;
    WORD string[8];
} sdSerial_t;

extern sdSerial_t sdSerial;

#endif //USBCFG_H
