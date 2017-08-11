//-------------------------------------------------------------------
#ifndef __ControllerDccpp_H__
#define __ControllerDccpp_H__
//-------------------------------------------------------------------

#include "Controller.hpp"

#ifdef VISUALSTUDIO1
#include "../VStudio/DCCpp.hpp"
#else
#include "DCCpp/DCCpp_Uno.h"
#include "DCCpp/PacketRegister.h"
#include "DCCpp/CurrentMonitor.h"
//#include "DCCpp/SerialCommand.h"
#include "DCCpp/Config.h"
//#include "DCCpp/Comm.h"
#endif

//-------------------------------------------------------------------

class ControllerDccpp : public Controller
{
	private:
		bool programMode;

		static CurrentMonitor *mMonitor;

		void SetSpeedRaw();
		void SetFunctionsRaw();

	public:
		static volatile RegisterList mainRegs, progRegs;

		ControllerDccpp();
		
	public:
		void begin();
		void beginMain(uint8_t OptionalDirectionMotor, uint8_t Dummy, uint8_t SignalEnablePin, uint8_t CurrentMonitor);
		void beginProg(uint8_t OptionalDirectionMotor, uint8_t Dummy, uint8_t SignalEnablePin, uint8_t CurrentMonitor);
		bool SetSpeed(int inNewSpeed);
		bool SetDirection(bool inToLeft);
		int ReadCv(byte inCv);
		void WriteCv(int inId, byte inCv);
		void SetFunction(byte inFunctionNumber, bool inActivate);
		void PanicStop(bool inStop);
		void loop();
		void StartProgramMode();
		void EndProgramMode();

		inline byte GetFunctionAddress(byte inFunctionNumber) const { return this->pControlled->Functions[inFunctionNumber].DccIdFunction; }
		inline void SetFunctionAddress(byte inFunctionNumber, byte inDccAddress) { this->pControlled->Functions[inFunctionNumber].DccIdFunction = inDccAddress; }

#ifdef DDC_DEBUG_MODE
		static void showConfiguration();
#endif
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
