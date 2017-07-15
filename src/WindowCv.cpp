/*************************************************************
project: <LcdUi>
author: <Thierry PARIS>
description: <Class for a Cv edition>
*************************************************************/

#include "DcDccNanoControler.h"
#include "WindowCv.hpp"

void WindowCv::begin(byte inFirstLine)
{
	this->Window::begin(inFirstLine);
}

void WindowCv::Event(byte inEventType, LcdUi *inpLcd)
{
	bool showValue = false;

	if (this->state == STATE_INITIALIZE)
	{
		this->state = STATE_NONE;
		this->valueEdited = false;
		this->undefValue = true;
		this->value = 0;
		//((ControlerDccpp *)DcDccControler::pControler)->StartProgramMode();
		showValue = true;
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
				if (this->undefValue)
					this->value = 0;
				else
					this->value++;
				this->undefValue = false;
			}
			else
			{
				this->cvAddress++;
				if (this->cvAddress > 1023)
					this->cvAddress = 1023;
			}
			showValue = true;
			break;
		case EVENT_LESS:
			if (this->valueEdited)
			{
				this->value--;
				this->undefValue = false;
			}
			else
			{
				this->cvAddress--;
				if (this->cvAddress < 0)
					this->cvAddress = 0;
			}
			showValue = true;
			break;
		case EVENT_SELECT:
			if (this->valueEdited)
			{
				if (!this->undefValue)
					((ControlerDccpp *)DcDccControler::pControler)->WriteCv(this->cvAddress, this->value);
			}
			else
			{
				this->valueEdited = true;
				int val = ((ControlerDccpp *)DcDccControler::pControler)->ReadCv(this->cvAddress);
				this->undefValue = (val < 0);
				if (val >= 0)
					this->value = val;
				else
					this->value = 0;

				showValue = true;
			}
			break;
		case EVENT_CANCEL:
			if (this->valueEdited)
			{
				this->valueEdited = false;
				showValue = true;
			}
			else
			{
				this->state = STATE_ABORTED;
				//((ControlerDccpp *)DcDccControler::pControler)->EndProgramMode();
			}
			break;
	}

	if (showValue)
	{
		inpLcd->GetScreen()->DisplayTextF(F("Cv"), 0, 1);
		if (!this->valueEdited)
		{
			inpLcd->GetScreen()->DisplayChar('>', 3, 1);
			inpLcd->GetScreen()->DisplayChar('<', 8, 1);
			inpLcd->GetScreen()->DisplayChar(' ', 11, 1);
			inpLcd->GetScreen()->DisplayChar(' ', 15, 1);
		}
		else
		{
			inpLcd->GetScreen()->DisplayChar(' ', 3, 1);
			inpLcd->GetScreen()->DisplayChar(' ', 8, 1);
			inpLcd->GetScreen()->DisplayChar('>', 11, 1);
			inpLcd->GetScreen()->DisplayChar('<', 15, 1);
		}
		LcdScreen::BuildString(this->cvAddress, LcdScreen::buffer, 4);
		inpLcd->GetScreen()->DisplayText(LcdScreen::buffer, 4, 1);
		inpLcd->GetScreen()->DisplayChar('=', 9, 1);
		if (this->undefValue)
			inpLcd->GetScreen()->DisplayText("---", 12, 1);
		else
		{
			LcdScreen::BuildString(this->value, LcdScreen::buffer, 3);
			inpLcd->GetScreen()->DisplayText(LcdScreen::buffer, 12, 1);
		}
	}
}

#ifdef LCDUI_PRINT_WINDOWS
void WindowCv::printWindow()
{
	printWindowHeader(F("Window Cv"));
	Serial.println("");
}
#endif