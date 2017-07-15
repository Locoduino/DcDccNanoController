/*************************************************************
project: <Dc/Dcc Controler>
author: <Thierry PARIS>
description: <Base functions of the library>
*************************************************************/

#include "DcDccNanoControler.h"
#include "Locomotive.hpp"
#include "EEPROM.h"		// only for EEPROM.length() !

DcDcc  DcDccControler::dcType;
DcDcc  DcDccControler::dcTypeAtStart;
Handle DcDccControler::handle;
//Handle ** DcDccControler::pHandleList;
//int  DcDccControler::handleAddcounter;
Controler * DcDccControler::pControler;
bool DcDccControler::beginFinished;

#ifdef DDC_DEBUG_MODE
void CheckPinNb(GPIO_pin_t inPin, const __FlashStringHelper *inFunc)
{
	return CheckPinNb(GPIO_to_Arduino_pin(inPin), inFunc);
}

void CheckPinNb(int inPin, const __FlashStringHelper *inFunc)
{
	if (inPin <= 0 || inPin >= NUM_DIGITAL_PINS)
	{
		Serial.print(F("Pin "));
		Serial.print(inPin);
		Serial.print(F(" error in "));
		Serial.println(inFunc);
	}
}
#endif

//#define DDC_DEBUG_MODE

void DcDccControler::begin(uint8_t inDcDccSelectPin, LcdScreen *apScreen)
{
#ifdef DDC_DEBUG_MODE
	Serial.println(F(""));
	Serial.println(F("Dc/Dcc Controler"));
	Serial.println(F("Developed by Thierry Paris."));
	Serial.println(F(""));

	Serial.println(F("*** Setup started."));
#endif

	//DcDccControler::pHandleList = NULL;
	//cDccControler::handleAddcounter = 0;
	DcDccControler::pControler = NULL;
	DcDccControler::beginFinished = false;

#ifdef DDC_DEBUG_MODE
	Serial.println(F("Setup started (Nano version)..."));
#endif
#ifndef VISUALSTUDIO
	//while (!Serial);
#endif

	DcDccControler::dcType = Dc;
	if (inDcDccSelectPin == 255)
		DcDccControler::dcType = Dcc;
	if (inDcDccSelectPin > 0 && inDcDccSelectPin < 255)
	{
		pinMode(inDcDccSelectPin, INPUT_PULLUP);
		if (digitalRead(inDcDccSelectPin) == HIGH)
			DcDccControler::dcType = Dcc;
	}

	// Just for let the time to the microC to initialize internals...
	delay(500);

	LcdScreen::YesMsg = STR_YES;
	LcdScreen::NoMsg = STR_NO;

	if (DcDccControler::dcType == Dc)
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("Start in Dc mode"));
#endif
		DcDccControler::pControler = new ControlerDc();
	}
	else
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("Start in Dcc mode"));
#endif
		DcDccControler::pControler = new ControlerDccpp();
	}

	DcDccControler::pControler->begin();
	DcDccControler::handle.begin();
	DcDccControler::handle.GetUI()->begin(apScreen);
}

void DcDccControler::beginMain(uint8_t DirectionMotor, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor)
{
	DcDccControler::pControler->beginMain(DirectionMotor, SignalPin, SignalEnablePin, CurrentMonitor);
}

void DcDccControler::beginProg(uint8_t DirectionMotor, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor)
{
	DcDccControler::pControler->beginProg(DirectionMotor, SignalPin, SignalEnablePin, CurrentMonitor);
}

void DcDccControler::beforeFirstLoop()
{
	if (DcDccControler::dcType == Dc)
	{
		// Affect a special loco to this handle.
		DcDccControler::handle.SetControledLocomotive(Locomotive::AnalogLocomotive);
		DcDccControler::handle.MoreLessIncrement = 10;
	}

	ConfigLoad();

	DcDccControler::handle.StartUI();
	DcDccControler::handle.StartContent();

	DcDccControler::handle.EndSetup();

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

void DcDccControler::loop(unsigned long inEvent, int inData)
{
	if (!DcDccControler::beginFinished)
	{
		DcDccControler::beforeFirstLoop();
		DcDccControler::beginFinished = true;
	}

	if (DcDccControler::pControler != 0)
		DcDccControler::pControler->loop();

	DcDccControler::handle.loop(inEvent, inData);
}

void DcDccControler::ConfigLoad()
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
		if (DcDccControler::dcType == Dc)
		{
			byte dCFrequencyDivisorIndex = EEPROM.read(3);

			if (dCFrequencyDivisorIndex < NB_PWM_FREQ_11_3)
				// Set frequency, even if the pin is not correct at this moment !
				((ControlerDc *)DcDccControler::pControler)->DCFrequencyDivisorIndex = dCFrequencyDivisorIndex;
		}

		// Must be done only when the good value is in Locomotive::FunctionNumber...
		DcDccControler::handle.ConfigLoad();
	}
}

int DcDccControler::ConfigSave()
{
	EEPROM.update(0, 'D');
	EEPROM.update(1, 'D');
	EEPROM.update(2, 'C');

	int pos = 4;
	if (DcDccControler::dcType == Dc)
		EEPROM.update(3, ((ControlerDc *)DcDccControler::pControler)->DCFrequencyDivisorIndex);
	else
	{
	}

	// Must be done only when the good value is in Locomotive::FunctionNumber...
	DcDccControler::handle.ConfigSave();

	return pos; // Pos of next writing...
}

void DcDccControler::ConfigReset()
{
	for (uint16_t i = 0; i < EEPROM.length(); i++)
		EEPROM.update(i, 0);
}