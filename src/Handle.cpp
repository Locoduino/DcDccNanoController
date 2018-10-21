/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Class for one handle>
*************************************************************/

#include "DcDccNanoController.h"
#ifndef NO_BUTTONSCOMMANDERPOTENTIOMETER
#include <Commanders.h>
#endif

#include <stdarg.h>
#include "EEPROM.h"

Handle::Handle()
{
	this->id = 0;
	this->MoreLessIncrement = 10;
	this->pUi = NULL;

	for (int i = 0; i < FUNCTION_NUMBER; i++)
		this->addressFunction[i] = 0;
}

void Handle::begin(byte inHandleId)
{
	this->id = inHandleId;
	if (this->pUi == 0)
		this->pUi = new LcdUi();
}

void Handle::StartUI()
{
#ifndef VISUALSTUDIO
//	extern uint8_t *__brkval;
//	extern uint8_t *__heap_start;
#endif

//	int v;

	if (this->pUi != 0)
	{
		/*
		In Dc :
		0	Splash
		1	Start : Confirm dc
		2		Mode Choice				Choice
		3			Config DDC			Choice
		4				PWM freq		WindowChooseDcFreq
		5			LocoControl			WindowLocoControl
		6	Stop						Interrupt

		In Dcc :
		0	Splash
		1	Start : Confirm dcc
		2		DDC menu				Choice
		3			Config DDC			Choice
		4				ID				EditInt	1-127 or 1-10126
		6				Steps			Choice 14/28/128
		7				Modify Cv		WindowCv
		8				Function n		EditInt	1-127 or 1-10126
		9				Function n+1	EditInt	1-127 or 1-10126
		10			LocoControl			WindowLocoControl
		11	Stop						Interrupt
		*/

		// BEGINS !
		winSplash.begin(STR_TITLE, STR_COPYRIGHT);
		winStart.begin(DcDccController::dcType == Dc?STR_DC:STR_DCC, STR_CONFIRM, NULL);
		winChoiceMain.begin(STR_MODEMODECHOICE, &choiceMain);	// menu
		winChoiceConfigDDC.begin(STR_MODECONFIG, &choiceConfig);
		if (DcDccController::dcType == Dc)
		{
			winFreq.begin(STR_PWMFREQCFG, NULL);
		}
		else
		{
			winLocoId.begin(STR_LOCOID, NULL, 1, 10026);
			winChoiceSteps.begin(STR_LOCOSTEPS, &choiceSteps);
			winFunctions.begin(STR_FUNCTIONS);
			winProgramCV.begin(STR_MODIFYCV);
		}

		winLocoControl.begin(254, this);
		this->windowInterruptEmergency.begin(STR_STOP, STR_STOP2, EVENT_EMERGENCY); // Emergency stop

		// WINDOWS in the list
		this->pUi->AddWindow(&winSplash);
		this->pUi->AddWindow(&winStart);
		this->pUi->AddWindow(&winChoiceMain);
		this->pUi->AddWindow(&winLocoControl);
		this->pUi->AddWindow(&winChoiceConfigDDC);
		if (DcDccController::dcType == Dc)
		{
			this->pUi->AddWindow(&winFreq);
		}
		else
		{
			this->pUi->AddWindow(&winLocoId);
			this->pUi->AddWindow(&winChoiceSteps);
			this->pUi->AddWindow(&winFunctions);
			this->pUi->AddWindow(&winProgramCV);
		}

		this->pUi->AddWindow(&this->windowInterruptEmergency);

		// CHOICES
		winChoiceMain.AddChoice(STR_MODELOCOCTRL, &winLocoControl); // run
		winChoiceMain.AddChoice(STR_MODECONFIG, &winChoiceConfigDDC);
		if (DcDccController::dcType == Dc)
		{
			winChoiceConfigDDC.AddChoice(STR_PWMFREQCFG, &winFreq);	// DC Freq
			winFreq.SetValueAddress(&(((ControllerDc *)DcDccController::pController)->DCFrequencyDivisorIndex));
		}
		else
		{
				winChoiceConfigDDC.AddChoice(STR_LOCOID, &winLocoId);
				winChoiceConfigDDC.AddChoice(STR_LOCOSTEPS, &winChoiceSteps);
					winChoiceSteps.AddChoice(STR_LOCOSTEPS14);
					winChoiceSteps.AddChoice(STR_LOCOSTEPS28);
					winChoiceSteps.AddChoice(STR_LOCOSTEPS128);
				winChoiceConfigDDC.AddChoice(STR_FUNCTIONS, &winFunctions);
				winChoiceConfigDDC.AddChoice(STR_MODIFYCV, &winProgramCV);
		}
	}

	winLocoId.SetValueAddress(this->controlled.GetDccIdAddress());

//	Serial.print((int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval));
//	Serial.println(F(" bytes"));

//	PRINT_WINDOWS(this->pUi);
}

void Handle::EndSetup()
{
	this->SetSpeed(0);
}

void Handle::StartContent()
{
}
	
void Handle::Clear()
{
	this->MoreLessIncrement = 10;
}

void Handle::SetControlledLocomotive(Locomotive &inLocomotive)
{
#ifdef DDC_DEBUG_MODE
	Serial.println(F("SetLocomotive"));
#endif
	this->controlled.Copy(inLocomotive);
}

bool Handle::loop(unsigned long inEvent, int inData)
{
	bool saveConfig = false;

	int event;

	if (inEvent <= LAST_HANDLED_EVENT)
	{	// Five handles events
		if (inEvent / 50 != this->id)
			return false;

		event = inEvent % 50;

		if (event == LCD1_EVENT_ENCODER)
		{
			if (inData == +1)
				event = EVENT_MORE;
			if (inData == -1)
				event = EVENT_LESS;
		}

		// Handle Function events
		if (event >= LCD1_EVENT_FUNCTION0)
		{
			byte function = event - LCD1_EVENT_FUNCTION0;
			bool activate = inData == COMMANDERS_MOVE_ON;

			if (DcDccController::dcType == Dcc)
			{
				this->SetFunction(function, activate);
#ifdef DDC_DEBUG_MODE
				Serial.print(F("Function F"));
				Serial.print(function);
				Serial.println(activate ? F(" On") : F(" Off"));
#endif
			}
			else
			{
				if (function == 0)
				{
					Window *pCurrent = pUi->GetGlobalCurrentWindow();
					((ControllerDc *)DcDccController::pController)->SetSlowMode(activate);
					int steps = ((ControllerDc *)DcDccController::pController)->GetMaxSpeed();
					this->MoreLessIncrement = steps / (pUi->GetScreen()->GetSizeX() - 2);
					pCurrent->SetState(STATE_START);
				}
			}
		}

	}
	else
		// Other event (emergency and others...)
		event = inEvent;

	if (pUi->loop(event))
	{
		Window *pCurrent = pUi->GetGlobalCurrentWindow();
		Locomotive &loco = this->controlled;

#ifdef DDC_DEBUG_MODE
		//LcdUi::printEvent(event, F("pUi->Loop"));
		//Window::printState(pUi->GetState(), F("pUi->Loop"));
#endif

		switch (pUi->GetState())
		{
		case STATE_INITIALIZE:
			int val;
			switch (pUi->GetWindowId())
			{
			case STR_STOP:
				DcDccController::pController->PanicStop(true);
				break;
			case STR_LOCOID:
				// Gets the current id of the loco...
				val = ((ControllerDccpp *)DcDccController::pController)->ReadCv(1);

				if (val < 0)
					val = 3;
				this->controlled.SetDccId(val);
				break;
			case STR_LOCOSTEPS:
				switch (loco.GetSteps())
				{
				case 14: val = STR_LOCOSTEPS14; break;
				case 28: val = STR_LOCOSTEPS28; break;
				default: val = STR_LOCOSTEPS128; break;
				}

				winChoiceSteps.SetCurrentChoiceById(val);
				break;
			}
			break;

		case STATE_CONFIRMED:
			switch (pUi->GetWindowId())
			{
			case STR_DC:
			case STR_DCC:
				DcDccController::pController->PanicStop(false);
				pCurrent->SetActive(false);
				break;
			case STR_STOP:
				DcDccController::pController->PanicStop(false);
				break;
			case STR_PWMFREQCFG:
				{
					WindowChooseDcFreq *pWinFreq = (WindowChooseDcFreq *)pCurrent;
					((ControllerDc *)DcDccController::pController)->SetFrequencyDivisorIndex(pWinFreq->GetChoiceValue());
					DcDccController::ConfigSave();
				}
				break;
			case STR_LOCOSTEPS:
				int val;
				switch (this->winChoiceSteps.GetFirstLine())
				{
				case STR_LOCOSTEPS14: val = 14; this->MoreLessIncrement = 1; break;
				case STR_LOCOSTEPS28: val = 28; this->MoreLessIncrement = 1; break;
				default: val = 128; this->MoreLessIncrement = 10; break;
				}
				loco.SetSteps(val);
				saveConfig = true;
				break;
			case STR_FUNCTIONS:
				saveConfig = true;
				break;

			case STR_LOCOID:
				// Refresh current state of functions for the new loco.
				DcDccController::pController->SetFunction(-1, true);
				//saveConfig = true;
				break;
			}
			pUi->SetState(STATE_POSTCONFIRMED);
			break;
		}

		if (saveConfig)
			this->ConfigSave();
	}

	return saveConfig;
}

void Handle::SetSpeed(int inNewSpeed)
{
	DcDccController::pController->SetControlled(&this->controlled);
	DcDccController::pController->SetSpeed(inNewSpeed);
}

void Handle::SetDirection(bool inToLeft)
{
	DcDccController::pController->SetControlled(&this->controlled);
	DcDccController::pController->SetDirection(inToLeft);
}

void Handle::SetFunction(byte inFunctionNumber, bool inActivate)
{
	DcDccController::pController->SetControlled(&this->controlled);
	DcDccController::pController->SetFunction(inFunctionNumber, inActivate);
}

void Handle::ConfigLoad()
{
	int pos= EEPROM_DDC_CONFIG_SIZE;

	if (DcDccController::dcType == Dcc)
	{
		this->MoreLessIncrement = EEPROM.read(pos + 1);

		this->controlled.Load(pos + 2);
	}
}

void Handle::ConfigSave()
{
	int pos = EEPROM_DDC_CONFIG_SIZE;

	if (DcDccController::dcType == Dcc)
	{
		EEPROM.update(pos, 0);		// Data version
		EEPROM.update(pos+1, this->MoreLessIncrement);		// Data version
		// Save/load the current loco.
		this->controlled.Save(pos+2);
	}
}