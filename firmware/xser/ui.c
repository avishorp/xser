#include <GenericTypeDefs.h>
#include "ui.h"
#include "lcd.h"

// UI States
#define UI_STATE_UNCONF         0  // Unconfigured
#define UI_STATE_NOPORTNUM      1  // No port number displayed
#define UI_STATE_PORTNUM_HOLD   2  // Port number displayed (don't change to activity)
#define UI_STATE_PORTNUM        3  // Port number displayed
#define UI_STATE_ACT            4  // Activity indicator displayed
#define UI_STATE_AUTOBAUD       5  // Auto baud rate detection

#define NO_PORT_NUMBER          0

unsigned char UI_State;
unsigned char UI_PrevState;

volatile unsigned char UI_PortNumber;
unsigned long timer;

#define SEC 4000
#define PORTNUM_TO_ACT_TIME     10*SEC
#define ACT_TO_PORTNUM_TIME     20*SEC
#define AUTOBAUD_PRESS_TIME     2*SEC



//////////// Prototypes (internal)
void UI_SwitchToPORTNUM(BOOL hold);
void UI_SwitchToNOPORTNUM();
void UI_SwitchToACT();
void UI_SwitchToAUTOBAUD();


void UI_Init()
{
    UI_State = UI_STATE_UNCONF;
    UI_PrevState = UI_STATE_UNCONF;
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
                UI_SwitchToPORTNUM(1);
            }
            if (events & (EVENT_TX_ACT | EVENT_RX_ACT)) {
                // Activity on the port, switch to activity
                // indicator mode
                UI_SwitchToACT();
            }

            break;

        case UI_STATE_PORTNUM_HOLD:
            // When port number is first shown, wait some time
            // before switching into activity indicator,
            // unless TX activity is present (which means that the port number
            // is already known to the user)
            if (events & (EVENT_TX_ACT) & 0) {
                // Activity on the port, switch to activity
                // indicator mode
                UI_SwitchToACT();
            }
            else if (timer == 0)
                UI_State = UI_STATE_PORTNUM;
            break;

        case UI_STATE_PORTNUM:
            if (events & (EVENT_TX_ACT | EVENT_RX_ACT)) {
                // Activity on the port, switch to activity
                // indicator mode
                UI_SwitchToACT();
            }
            break;

        case UI_STATE_ACT:
            if (events & EVENT_HIDCMD) {
                // Port number set command has been received
                UI_SwitchToPORTNUM(1);
            }
            else if (timer > 0) {
                // Update activity indicators
                LCD_SetDisplayValue(events & 0x03);
                if ((events & 0x03) != 0)
                    timer = ACT_TO_PORTNUM_TIME;
            }
            else {
               if (UI_PortNumber != NO_PORT_NUMBER)
                    // After long inacticity, switch to port
                    // numbwe display
                    UI_SwitchToPORTNUM(0);
            }
            break;
    }
}

void UI_SetPortNumber(unsigned char portnum)
{
    UI_PortNumber = portnum;
}

void UI_SwitchToPORTNUM(BOOL hold)
{
    UI_PrevState = UI_State;

    if (hold)
        UI_State = UI_STATE_PORTNUM_HOLD;
    else
        UI_State = UI_STATE_PORTNUM;
    LCD_SetDisplayType(DISP_TYPE_NUMBER);
    LCD_SetDisplayValue(UI_PortNumber);
    timer = PORTNUM_TO_ACT_TIME;
}

void UI_SwitchToNOPORTNUM()
{
    UI_PrevState = UI_State;

    UI_State = UI_STATE_NOPORTNUM;
    LCD_SetDisplayType(DISP_TYPE_WAIT);
}

void UI_SwitchToACT()
{
    UI_PrevState = UI_State;

    UI_State = UI_STATE_ACT;
    LCD_SetDisplayType(DISP_TYPE_ACT);
    timer = ACT_TO_PORTNUM_TIME;
}
