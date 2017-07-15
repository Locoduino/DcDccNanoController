/*************************************************************
project: <Accessories>
author: <Thierry PARIS>
description: <2 servos and 3 leds driven by dcc arduino>
*************************************************************/

#include <LcdUi.h>

#include <Commanders.h>

#define kDCC_INTERRUPT      0

ButtonsCommanderPush push;
ButtonsCommanderKeyboard keyb0, keyb1, keyb2, keyb4, keyb5;
ButtonsCommanderSwitch swiitch;	// two 'ii' to avoid collision with the C word 'switch' !
ButtonsCommanderPotentiometer potar; //(321, 0, 20, 145)	// Link it to SERVO1, from 20 to 145 degrees

SERIAL_COMMANDER(Serial);

// Accessories

WindowInt test;

//////////////////////////////////
//
// Setup
//
void setup()
{
	Serial.begin(115200);

	Commanders::begin(LED_BUILTIN);

	// Setup of Dcc commander
	DccCommander.begin(0x00, 0x00, kDCC_INTERRUPT);
	SerialCommander.begin();

	keyb0.begin(1000, '0');
	keyb1.begin(DCCINT(319, 0), '1');
	keyb2.begin(DCCINT(319, 1), '2');

	keyb4.begin(DCCINT(320, 0), '4');
	keyb5.begin(DCCINT(320, 1), '5');

	// The push button send two Dcc codes in turn, and is connected to pin 26.
	push.begin(1000, 26);	// id 1000, pin 26
	push.AddEvent(DCCINT(320, 0));
	push.AddEvent(DCCINT(320, 1));

	// The switch have two positions, each one controls one Dcc code. Each one connected to its pin.
	swiitch.begin();
	swiitch.AddEvent(24, DCCINT(319, 0));
	swiitch.AddEvent(25, DCCINT(319, 1));

	// The potentiometer is on analog pin 8.
	potar.begin(8, 1001, 0, 100);
}

void loop()
{
	Commanders::loop();
	//LcdUi::loop();
}
