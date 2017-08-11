/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Class for one locomotive>
*************************************************************/

#include "DcDccNanoController.h"
#include "Locomotive.hpp"
#include "EEPROM.h"

#include <stdarg.h>

#ifdef DDC_DEBUG_MODE
#define CHECK(val, text)	CheckIndex(val, F(text))

void Locomotive::CheckIndex(byte inIndex, const __FlashStringHelper *inFunc)
{
	if (inIndex >= FUNCTION_NUMBER)
	{
		Serial.print(F("Index error in "));
		Serial.println(inFunc);
	}
}
#else
#define CHECK(val, text)
#endif

Locomotive Locomotive::AnalogLocomotive(1, 0, (char *)"", 255);

Locomotive::Locomotive()
{
	Clear();
}

Locomotive::Locomotive(int inDccId, uint8_t inAdressKind, char *inName, uint8_t inSteps) : Locomotive()
{
	this->SlotNumber = 255;
	this->DccId = inDccId;
	this->steps = inSteps;
}

void Locomotive::ClearFunctions()
{
	// Delete all existing functions
	for (int i = 0; i < FUNCTION_NUMBER; i++)
	{
		this->Functions[i].Clear();
	}
}

byte Locomotive::GetFunctionIndex(const int inDccId) const
{
	for (int i = 0; i < FUNCTION_NUMBER; i++)
	{
		if (this->Functions[i].DccIdFunction == inDccId)
			return i;
	}

	return 255;
}

void Locomotive::Copy(Locomotive &inLocomotive)
{
	this->SlotNumber = inLocomotive.SlotNumber;
	this->DccId = inLocomotive.DccId;
	this->steps = inLocomotive.steps;

	for (int f = 0; f < FUNCTION_NUMBER; f++)
		this->Functions[f].Copy(inLocomotive.Functions[f]);
}

void Locomotive::Load(int inStartPos)
{
	eeprom_read_block((void*)&this->DccId, (const void *)inStartPos, sizeof(int));
	inStartPos += sizeof(int);

	if (this->DccId == 0)
	{
		this->Clear();
		return;
	}

	this->steps = EEPROM.read(inStartPos++);
	for (int f = 0; f < FUNCTION_NUMBER; f++)
		this->Functions[f].Load(inStartPos + 10 + f * 10);

#ifdef DDC_DEBUG_MODE
	this->print();
#endif
}

void Locomotive::Save(int inStartPos)
{
	eeprom_update_block((const void*)&this->DccId, (void *)inStartPos, sizeof(int));
	inStartPos += sizeof(int);
	EEPROM.update(inStartPos++, this->steps);

	for (int f = 0; f < FUNCTION_NUMBER; f++)
		this->Functions[f].Save(inStartPos + 10 + f * 10);
}

void Locomotive::Clear()
{
	this->SlotNumber = 255;

	this->DccId = 3;
	this->steps = 128;
	this->directionToLeft = true;
	this->ClearFunctions();
}

#ifdef DDC_DEBUG_MODE
void Locomotive::print()
{
	Serial.print("Loco Id ");
	Serial.print(this->DccId);
	Serial.print(" / Steps ");
	Serial.print(this->steps);

	for (int f = 0; f < FUNCTION_NUMBER; f++)
	{
		Serial.print(" / F");
		Serial.print(f);
		Serial.print(" = ");
		Serial.print(this->Functions[f].DccIdFunction);
	}

	Serial.println("");
}
#endif