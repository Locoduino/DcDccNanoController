/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Dc Controller>
*************************************************************/

#include "DcDccNanoController.h"
#include "ControllerDc.hpp"

//													1024 256 128 64	32	8  1  ---
const byte ControllerDc::divisors_11_3_pow2[]{ 10,  8,  7, 6, 5, 3, 0, 255 };

void ControllerDc::begin()
{ 
}

void ControllerDc::beginMain(uint8_t inDummy, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor)
{
	this->dcPWMpin = SignalEnablePin;
	this->dcDirPin = SignalPin;
	this->dcCurrentMonitor = CurrentMonitor;

	pinMode(this->dcPWMpin, OUTPUT);
	pinMode(this->dcDirPin, OUTPUT);
	if (CurrentMonitor != 255)
		pinMode(this->dcCurrentMonitor, INPUT);
	analogWrite(this->dcPWMpin, 0);
	SetFrequencyDivisorRaw(ControllerDc::GetFrequencyDivisor(this->DCFrequencyDivisorIndex));

#ifdef DDC_DEBUG_MODE
	Serial.println(F("Dc mode."));
#endif
}

bool ControllerDc::SetSpeed(int inNewSpeed)
{
	int val;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDc SetSpeed "));
	Serial.println(inNewSpeed);
#endif

	val = map(inNewSpeed, 0, this->pControlled->GetSteps(), 0, 255);

	if (this->pControlled->GetMappedSpeed() == val)
		return false;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("analogWrite pin "));
	Serial.print(this->dcPWMpin);
	Serial.print(F(" / value "));
	Serial.println(val);
#endif
	analogWrite(this->dcPWMpin, val);
	if (this->pControlled->GetDirectionToLeft())
		digitalWrite(this->dcDirPin, LOW);
	else
		digitalWrite(this->dcDirPin, HIGH);

	this->pControlled->SetMappedSpeed(val);

	return true;
}

void ControllerDc::SetSlowMode(bool inSlowMode) 
{ 
	this->slowMode = inSlowMode; 
	this->maxSpeed = inSlowMode ? SLOWMODELIMIT : 255; 

	if (this->pControlled->GetMappedSpeed() > this->maxSpeed)
		SetSpeed(this->maxSpeed);
}


bool ControllerDc::SetDirection(bool inToLeft)
{
	if (this->pControlled->GetDirectionToLeft() == inToLeft)
		return false;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDc SetDirection "));
	Serial.println(inToLeft ? F("ToLeft"):F("ToRight"));
#endif

	this->pControlled->SetDirectionToLeft(inToLeft);
	this->SetSpeed(0);
	return true;
}

void ControllerDc::PanicStop(bool inStop) 
{
	if (this->panicStopped == inStop)
		return;

	this->panicStopped = inStop;
	if (inStop)
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("ControllerDc PanicStop pressed"));
#endif

		analogWrite(this->dcPWMpin, 0);
	}
	else
	{
#ifdef DDC_DEBUG_MODE
		Serial.println(F("ControllerDc PanicStop canceled"));
#endif
		analogWrite(this->dcPWMpin, this->pControlled->GetMappedSpeed());
		if (this->pControlled->GetDirectionToLeft())
			digitalWrite(this->dcDirPin, LOW);
		else
			digitalWrite(this->dcDirPin, HIGH);
	}
}

void ControllerDc::BuildFreqString(unsigned int inDivisor)
{
	unsigned int val = BASE_PWM_FREQ_11_3 / inDivisor;

	LcdScreen::BuildString(val, LcdScreen::buffer);

	size_t len = strlen(LcdScreen::buffer);
	LcdScreen::buffer[len++] = ' ';
	LcdScreen::buffer[len++] = 'H';
	LcdScreen::buffer[len++] = 'z';
	LcdScreen::buffer[len] = 0;
}

void ControllerDc::SetFrequencyDivisorIndex(byte inDivisorIndex)
{
	this->DCFrequencyDivisorIndex = inDivisorIndex;

	this->SetFrequencyDivisorRaw(1 << ControllerDc::divisors_11_3_pow2[inDivisorIndex]);
}

void ControllerDc::SetFrequencyDivisorRaw(unsigned int inDivisor)
{
	byte mode;
	if (this->dcPWMpin == 5 || this->dcPWMpin == 6 || this->dcPWMpin == 9 || this->dcPWMpin == 10) 
	{
		switch (inDivisor) 
		{
			case 1: mode = 0x01; break;
			case 8: mode = 0x02; break;
			case 64: mode = 0x03; break;
			case 256: mode = 0x04; break;
			case 1024: mode = 0x05; break;
			default: return;
		}
		if (this->dcPWMpin == 5 || this->dcPWMpin == 6)
			TCCR0B = (TCCR0B & 0b11111000) | mode;
		else
			TCCR1B = (TCCR1B & 0b11111000) | mode;
		return;
	}

	// Only pin 3 or 11 are allowed for PWM pin.
	if (this->dcPWMpin == 3 || this->dcPWMpin == 11) 
	{
#ifdef DDC_DEBUG_MODE
		Serial.print(F("Change DC freq divisor: "));
		Serial.println(inDivisor);
#endif
		switch (inDivisor) 
		{
			case 1: mode = 0x01; break;
			case 8: mode = 0x02; break;
			case 32: mode = 0x03; break;
			case 64: mode = 0x04; break;
			case 128: mode = 0x05; break;
			case 256: mode = 0x06; break;
			case 1024: mode = 0x7; break;
			default: return;
		}
		TCCR2B = (TCCR2B & 0b11111000) | mode;
	}
}


