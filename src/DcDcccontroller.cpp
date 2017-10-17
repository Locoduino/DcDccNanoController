/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Base functions of the library>
*************************************************************/

#include "DcDccNanoController.h"
#include "Locomotive.hpp"
#include "EEPROM.h"		// only for EEPROM.length() !

DcDcc  DcDccController::dcType;
Handle DcDccController::handle;
Controller * DcDccController::pController;
bool DcDccController::beginFinished;

void DcDccController::begin(uint8_t inDcDccSelectPin, LcdScreen *apScreen)
{
#ifdef DDC_DEBUG_MODE
	Serial.println(F(""));
	Serial.println(F("Dc/Dcc Nano Controller"));
	Serial.println(F("Developed by Thierry Paris."));
	Serial.println(F(""));

	Serial.println(F("*** Setup started."));
#endif

	//DcDccController::pHandleList = NULL;
	//cDccController::handleAddcounter = 0;
	DcDccController::pController = NULL;
	DcDccController::beginFinished = false;

#ifdef DDC_DEBUG_MODE
	Serial.println(F("Setup started..."));
#endif
#ifndef VISUALSTUDIO
	//while (!Serial);
#endif

	DcDccController::dcType = Dc;
	if (inDcDccSelectPin == 255)
		DcDccController::dcType = Dcc;
	if (inDcDccSelectPin > 0 && inDcDccSelectPin < 255)
	{
		pinMode(inDcDccSelectPin, INPUT_PULLUP);
		if (digitalRead(inDcDccSelectPin) == HIGH)
			DcDccController::dcType = Dcc;
	}

	// Just for let the time to the microC to initialize internals...
	delay(500);

	LcdScreen::YesMsg = STR_YES;
	LcdScreen::NoMsg = STR_NO;

	if (DcDccController::dcType == Dc)
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("Start in Dc mode"));
#endif
		DcDccController::pController = new ControllerDc();
	}
	else
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("Start in Dcc mode"));
#endif
		DcDccController::pController = new ControllerDccpp();
	}

	DcDccController::pController->begin();
	DcDccController::handle.begin();
	DcDccController::handle.GetUI()->begin(apScreen);
}

void DcDccController::beginMain(uint8_t DirectionMotor, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor)
{
	DcDccController::pController->beginMain(DirectionMotor, SignalPin, SignalEnablePin, CurrentMonitor);
}

void DcDccController::beginProg(uint8_t DirectionMotor, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor)
{
	DcDccController::pController->beginProg(DirectionMotor, SignalPin, SignalEnablePin, CurrentMonitor);
}

void DcDccController::beforeFirstLoop()
{
	if (DcDccController::dcType == Dc)
	{
		// Affect a special loco to this handle.
		DcDccController::handle.SetControlledLocomotive(Locomotive::AnalogLocomotive);
		DcDccController::handle.MoreLessIncrement = 10;
	}

	ConfigLoad();

	DcDccController::handle.StartUI();
	DcDccController::handle.StartContent();

	DcDccController::handle.EndSetup();

#ifdef DDC_DEBUG_MODE
	Serial.print(F("*** Setup Finished."));
#ifndef VISUALSTUDIO
	extern uint8_t *__brkval;
	extern uint8_t *__heap_start;

	Serial.print(F("   Memory used = "));
	Serial.print((int)(__brkval == 0 ? (int)&__heap_start : (int)__brkval) - (int)&__heap_start);
	Serial.println(F(" bytes"));
#endif
	Serial.println("");
#endif
}

void DcDccController::loop(unsigned long inEvent, int inData)
{
	if (!DcDccController::beginFinished)
	{
		DcDccController::beforeFirstLoop();
		DcDccController::beginFinished = true;
	}

	if (DcDccController::pController != 0)
		DcDccController::pController->loop();

	DcDccController::handle.loop(inEvent, inData);
}

void DcDccController::ConfigLoad()
{
	// The first three bytes must be 'DDC' to assume that there is already a saved configuration
	// in the EEPROM. If not, reset all the EEPROM to 0, and save the 'DDC' identifier.
	if (EEPROM.read(0) != 'D' || EEPROM.read(1) != 'D' || EEPROM.read(2) != 'C')
	{
		ConfigReset();
		ConfigSave();
	}
	else
	{
		if (DcDccController::dcType == Dc)
		{
			byte dCFrequencyDivisorIndex = EEPROM.read(3);

			if (dCFrequencyDivisorIndex < NB_PWM_FREQ_11_3)
				// Set frequency, even if the pin is not correct at this moment !
				((ControllerDc *)DcDccController::pController)->DCFrequencyDivisorIndex = dCFrequencyDivisorIndex;
		}

		// Must be done only when the good value is in Locomotive::FunctionNumber...
		DcDccController::handle.ConfigLoad();
	}
}

int DcDccController::ConfigSave()
{
	EEPROM.update(0, 'D');
	EEPROM.update(1, 'D');
	EEPROM.update(2, 'C');

	int pos = 4;
	if (DcDccController::dcType == Dc)
		EEPROM.update(3, ((ControllerDc *)DcDccController::pController)->DCFrequencyDivisorIndex);
	else
	{
	}

	// Must be done only when the good value is in Locomotive::FunctionNumber...
	DcDccController::handle.ConfigSave();

	return pos; // Pos of next writing...
}

void DcDccController::ConfigReset()
{
	for (uint16_t i = 0; i < EEPROM.length(); i++)
		EEPROM.update(i, 0);
}