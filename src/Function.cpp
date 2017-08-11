/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Class for a decoder function>
*************************************************************/

#include "EEPROM.h"

#include "DcDccNanoController.h"
#include "Function.hpp"

Function::Function(int inDccId)
{
	this->SlotNumber = 255;
	this->DccIdFunction = inDccId;
	this->activated = false;
}

void Function::Clear()
{
	this->SlotNumber = 255;
	this->DccIdFunction = 0;
	this->activated = false;
}

void Function::Copy(const Function &inFunction)
{
	this->SlotNumber = inFunction.SlotNumber;
	this->DccIdFunction = inFunction.DccIdFunction;
	this->activated = inFunction.activated;
}

void Function::Load(int inStartPos)
{
	eeprom_read_block((void*)&this->DccIdFunction, (const void *)inStartPos, sizeof(int));
	inStartPos += sizeof(int);
	this->activated = EEPROM.read(inStartPos++)!=0;
}

void Function::Save(int inStartPos)
{
	eeprom_update_block((const void *)&this->DccIdFunction, (void *)inStartPos, sizeof(int));
	inStartPos += sizeof(int);
	EEPROM.update(inStartPos++, this->activated);
}
