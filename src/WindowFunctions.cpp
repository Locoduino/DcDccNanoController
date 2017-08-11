/*************************************************************
project: <LcdUi>
author: <Thierry PARIS>
description: <Class for a Cv edition>
*************************************************************/

#include "DcDccNanoController.h"
#include "WindowFunctions.hpp"

void WindowFunctions::begin(byte inFirstLine)
{
	this->Window::begin(inFirstLine);
}

void WindowFunctions::Event(byte inEventType, LcdUi *inpLcd)
{
	bool refresh = false;
	bool refreshValue = false;

	if (this->state == STATE_INITIALIZE)
	{
		this->state = STATE_NONE;
		this->valueEdited = false;
		this->value = 0;
		refresh = true;
		refreshValue = true;
	}

	if (this->state == STATE_START)
	{
		inpLcd->GetScreen()->DisplayHeader(this->firstLine);

		this->state = STATE_INITIALIZE;
	}

	switch (inEventType)
	{
		case EVENT_MORE:
			if (this->valueEdited)
			{
				this->value++;
				if (this->value > 28)
					this->value = 28;
			}
			else
			{
				this->fAddress++;
				if (this->fAddress >= FUNCTION_NUMBER)
					this->fAddress = FUNCTION_NUMBER - 1;
				refreshValue = true;
			}
			refresh = true;
			break;
		case EVENT_LESS:
			if (this->valueEdited)
			{
				this->value--;
				if (this->value > 28)
					this->value = 28;
			}
			else
			{
				this->fAddress--;
				if (this->fAddress < 0)
					this->fAddress = 0;
				refreshValue = true;
			}
			refresh = true;
			break;
		case EVENT_SELECT:
			if (!this->valueEdited)
			{
				this->valueEdited = true;
				refresh = true;
			}
			else
			{
				((ControllerDccpp *)DcDccController::pController)->SetFunctionAddress(this->fAddress, this->value);
				this->valueEdited = false;
				refresh = true;
			}
			break;
		case EVENT_CANCEL:
			if (this->valueEdited)
			{
				this->valueEdited = false;
				refresh = true;
			}
			else
			{
				this->state = STATE_CONFIRMED;
			}
			break;
	}

	if (refreshValue)
		this->value = ((ControllerDccpp *)DcDccController::pController)->GetFunctionAddress(this->fAddress);

	if (refresh)
	{
		inpLcd->GetScreen()->DisplayTextF(F("F"), 0, 1);
		char fs, fe, vs, ve;
		if (!this->valueEdited)
		{
			fs = '>';
			fe = '<';
			vs = ve = ' ';
		}
		else
		{
			fs = fe = ' ';
			vs = '>';
			ve = '<';
		}
		inpLcd->GetScreen()->DisplayChar(fs, 5, 1);
		inpLcd->GetScreen()->DisplayChar('0' + this->fAddress, 6, 1);
		inpLcd->GetScreen()->DisplayChar(fe, 7, 1);

		inpLcd->GetScreen()->DisplayChar('=', 9, 1);

		inpLcd->GetScreen()->DisplayChar(vs, 11, 1);
		inpLcd->GetScreen()->DisplayChar('0' + this->value / 10, 12, 1);	// tenths
		inpLcd->GetScreen()->DisplayChar('0' + this->value % 10, 13, 1);	// units
		inpLcd->GetScreen()->DisplayChar(ve, 14, 1);
	}
}

#ifdef LCDUI_PRINT_WINDOWS
void WindowFunctions::printWindow()
{
	printWindowHeader(F("Window Functions"));
	Serial.println(F(""));
}
#endif