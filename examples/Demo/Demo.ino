/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Dc/Dcc Nano Controller sample>
*************************************************************/

#include "French16.h"
#include <Commanders.h>

#include "DcDccNanoController.h"

#include <LiquidCrystal.h>
#include "ScreenLiquid.hpp"		// DcDccNaNoController include file associated with LiquidCrystal...

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
ScreenLiquid screen;

ButtonsCommanderPush buttonSelect;
ButtonsCommanderEncoder buttonEncoder;
ButtonsCommanderPush buttonCancel;
ButtonsCommanderPush buttonEmergency;
ButtonsCommanderSwitchOnePin buttonF0;
ButtonsCommanderSwitchOnePin buttonF1;

void setup()
{
	Serial.begin(115200);

	buttonSelect.begin(EVENT_SELECT, A0);
	buttonF0.begin(EVENT_FUNCTION0, A1);
	buttonF1.begin(EVENT_FUNCTION1, A2);
	buttonEmergency.begin(EVENT_EMERGENCY, A3);
	buttonCancel.begin(EVENT_CANCEL, A4);
	buttonEncoder.begin(EVENT_ENCODER, 12, 8, 2);

	screen.begin(16, 2, DcDccStringTable, &lcd);

	// if dcdcc pin equals to 255, dcc mode is forced.
	// if dcdcc pin equals to 0, dc mode is forced.
	// otherwise, pin state give dc or dcc.
	DcDccController::begin(A5, &screen);
	DcDccController::beginMain(255, DCC_SIGNAL_PIN_MAIN, 11, A6);
}

void loop()
{
	unsigned long eventId = Commanders::loop();

	// For LcdUi, UNDEFINED_ID of Commanders has no meaning. So convert it to EVENT_NONE.
	if (eventId == UNDEFINED_ID)
		eventId = EVENT_NONE;

	DcDccController::loop(eventId, Commanders::GetLastEventData());
}

