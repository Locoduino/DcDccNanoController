/*************************************************************
project: <Dc/Dcc Controler>
author: <Thierry PARIS>
description: <Dc/Dcc Nano controler sample>
*************************************************************/

#include "French16.h"
#include "Commanders.h"

#include "DcDccNanoControler.h"

#include <LiquidCrystal.h>
#include "ScreenLiquid.hpp"

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
ScreenLiquid screen;

/*
//#include <NewLiquidCrystal_I2C.h>
#include <NewLiquidCrystal.h>
#include "ScreenLiquidNew.hpp"

//NewLiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
NewLiquidCrystal lcd(8, 9, 4, 5, 6, 7);
ScreenLiquidNew screen;
*/

#ifdef VISUALSTUDIO
ButtonsCommanderKeyboard push0;
ButtonsCommanderKeyboard push1;
ButtonsCommanderKeyboard push2;
ButtonsCommanderKeyboard push3;
ButtonsCommanderKeyboard pushEmergency;
ButtonsCommanderKeyboard pushFunction1;
ButtonsCommanderKeyboard pushFunction2;
#endif
ButtonsCommanderPush buttonSelect;
ButtonsCommanderEncoder buttonEncoder;
ButtonsCommanderPush buttonCancel;
ButtonsCommanderPush buttonEmergency;
ButtonsCommanderSwitchOnePin buttonF0;
ButtonsCommanderSwitchOnePin buttonF1;

void setup()
{
	Serial.begin(115200);

#ifdef VISUALSTUDIO
	push0.begin(EVENT_SELECT, '*');
	push1.begin(EVENT_MORE, '+');
	push2.begin(EVENT_LESS, '-');
	push3.begin(EVENT_CANCEL, '/');
	pushEmergency.begin(EVENT_EMERGENCY, '0');
	pushFunction1.begin(EVENT_FUNCTION0, '1', COMMANDERS_EVENT_MOVE, COMMANDERS_MOVE_ON);
	pushFunction1.AddEvent(EVENT_FUNCTION0, COMMANDERS_EVENT_MOVE, COMMANDERS_MOVE_OFF);
	pushFunction2.begin(EVENT_FUNCTION1, '2', COMMANDERS_EVENT_MOVE, COMMANDERS_MOVE_ON);
	pushFunction2.AddEvent(EVENT_FUNCTION1, COMMANDERS_EVENT_MOVE, COMMANDERS_MOVE_OFF);
#endif
	buttonSelect.begin(EVENT_SELECT, A0);
	buttonEncoder.begin(EVENT_ENCODER, 12, 8, 2);
	buttonCancel.begin(EVENT_CANCEL, A3);
	buttonEmergency.begin(EVENT_EMERGENCY, A4);
	buttonF0.begin(EVENT_FUNCTION0, A1);
	buttonF1.begin(EVENT_FUNCTION1, A2);

#ifdef VISUALSTUDIO
	pinName(A0, "OK");
	pinName(A1, "F0");
	pinName(A2, "F1");
	pinName(A3, "CANC");
	pinName(A4, "STOP");
	pinName(A5, "DCC");
#endif

	// if dcdcc pin equals to 255, dcc mode is forced.
	// if dcdcc pin equals to 0, dc mode is forced.
	// otherwise, pin state give dc or dcc.
	screen.begin(16, 2, DcDccStringTable, &lcd);
	DcDccControler::begin(255/*A5*/, &screen);
	DcDccControler::beginMain(255, DCC_SIGNAL_PIN_MAIN, 11, A6);    // Dc: Dir, Pwm, current sensor
}

void loop()
{
	unsigned long eventId = Commanders::loop();

	// For LcdUi, UNDEFINED_ID of Commanders has no meaning. And because it is necessary 
	// to execute lcdui.event() at each call of the main loop, 
	// do it with an empty event : EVENT_NONE.

	if (eventId == UNDEFINED_ID)
		eventId = EVENT_NONE;

	DcDccControler::loop(eventId, Commanders::GetLastEventData());
}

