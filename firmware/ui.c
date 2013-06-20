#include "ui.h"
#include "lcd.h"

// UI States
#define UI_STATE_UNCONF         0  // Unconfigured
#define UI_STATE_NOPORTNUM      1  // Port number displayed
#define UI_STATE_PORTNUM        2  // Port number displayed
#define UI_STATE_ACT            3  // Activity indicator displayed

#define NO_PORT_NUMBER          0

unsigned char UI_State;
unsigned char UI_PortNumber;
unsigned int timer;

#define PORTNUM_TO_ACT_TIME     1000
#define ACT_TO_PORTNUM_TIME     5000



//////////// Prototypes (internal)
void UI_SwitchToPORTNUM();
void UI_SwitchToNOPORTNUM();
void UI_SwitchToACT();


void UI_Init()
{
    UI_State = UI_STATE_UNCONF;
    UI_PortNumber = NO_PORT_NUMBER;

    LCD_Init();
    LCD_SetDisplayType(DISP_TYPE_TEST);
}

void UI_Service(unsigned char events) {
    // Update timer
    if (timer > 0)
        timer--;

    // Handle state transitions
    switch (UI_State) {
        case UI_STATE_UNCONF:
            if (events & EVENT_USBCONF) {
                // USB has been configured
                UI_SwitchToNOPORTNUM();
            }
            break;

        case UI_STATE_NOPORTNUM:
            if (events & EVENT_HIDCMD) {
                // Port number set command has been received
                UI_SwitchToPORTNUM();
            }
            if (events & (EVENT_TX_ACT | EVENT_RX_ACT)) {
                // Activity on the port, switch to activity
                // indicator mode
                UI_SwitchToACT();
            }

            break;

        case UI_STATE_PORTNUM:
            if (timer == 0) {
                // When port number is first shown, wait some time
                // before switching into activity indicator
                
                if (events & (EVENT_TX_ACT | EVENT_RX_ACT)) {
                    // Activity on the port, switch to activity
                    // indicator mode
                    UI_SwitchToACT();
                }
            }
            break;

        case UI_STATE_ACT:
            if (timer > 0) {
                // Update activity indicators
                LCD_SetDisplayValue(events & 0x03);
                timer = ACT_TO_PORTNUM_TIME;
            }
            else
                if (UI_PortNumber != NO_PORT_NUMBER)
                    // After long inacticity, switch to port
                    // numbwe display
                    UI_SwitchToPORTNUM();
            break;
    }
}

void UI_SetPortNumber(unsigned char portnum)
{
    UI_PortNumber = portnum;
}

void UI_SwitchToPORTNUM()
{
    UI_State = UI_STATE_PORTNUM;
    LCD_SetDisplayType(DISP_TYPE_NUMBER);
    LCD_SetDisplayValue(UI_PortNumber);
    timer = PORTNUM_TO_ACT_TIME;
}

void UI_SwitchToNOPORTNUM()
{
    UI_State = UI_STATE_NOPORTNUM;
    LCD_SetDisplayType(DISP_TYPE_WAIT);
}

void UI_SwitchToACT()
{
    UI_State = UI_STATE_ACT;
    LCD_SetDisplayType(DISP_TYPE_ACT);
    timer = ACT_TO_PORTNUM_TIME;
}
