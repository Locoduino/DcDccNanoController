//-------------------------------------------------------------------
#ifndef __handle_H__
#define __handle_H__
//-------------------------------------------------------------------

#include "arduino.h"

#ifndef __LcdUi_H__
#include "LcdUi.h"
#endif

#include "Locomotive.hpp"
#include "Controller.hpp"

class Handle;

#include "WindowChooseDcFreq.hpp"
#include "WindowLocoControl.hpp"
#include "WindowFunctions.hpp"
#include "WindowCv.hpp"

//-------------------------------------------------------------------

#define EEPROM_DDC_HANDLE_CONFIG_SIZE	16

class Handle
{
	public:
		// Handle definition
		byte id;

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
		4				ID				EditInt	1-10126
		5				Steps			Choice 14/28/128
		6				Modify Cv		WindowCv
		7				Functions		EditInt	0-28
		8			LocoControl			WindowLocoControl
		9	Stop						Interrupt
		*/

		Choice choiceMain;
		Choice choiceConfig;
		Choice choiceConfigLoco;
		Choice choiceSteps;

		WindowSplash winSplash;
		WindowConfirm winStart;
		WindowChoice winChoiceMain;
		WindowChoice winChoiceConfigDDC;
		WindowChooseDcFreq winFreq;
		WindowInt winLocoId;
		WindowChoice winChoiceSteps;
		WindowFunctions winFunctions;
		WindowCv winProgramCV;
		WindowLocoControl winLocoControl;

		WindowInterrupt windowInterruptEmergency;
		int addressFunction[FUNCTION_NUMBER];

		LcdUi *pUi;

		// Handle configuration
		int MoreLessIncrement;

	private:
		// Handle interactive situation
		// In Mega version, because each handle moves its loco, RollingStock::Current cannot by used...
		Locomotive controlled;

	public:
		Handle();
		
		void begin(byte inHandleId = 0);
		void EndSetup();
		void StartUI();
		void StartContent();
		void Clear();

		inline LcdUi *GetUI() const { return this->pUi; }
		//void SetControlledLocomotive(byte inRollingStockSlot);
		void SetControlledLocomotive(Locomotive &Locomotive);
		inline const Locomotive &GetControlledLocomotive() const { return this->controlled; }

		inline bool IsLeftDir() const { return this->controlled.GetDirectionToLeft(); }
		inline bool IsRightDir() const { return !IsLeftDir(); }

		void SetSpeed(int inNewSpeed);
		void SetDirection(bool inToLeft);
		void SetFunction(byte inFunctionNumber, bool inActivate);

		bool loop(unsigned long inEvent, int inData);

		void ConfigLoad();
		void ConfigSave();

	private:
#ifdef DDC_DEBUG_MODE
		void CheckIndex(int inIndex, const __FlashStringHelper *infunc);
#endif
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
