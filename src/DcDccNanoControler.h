//-------------------------------------------------------------------
#ifndef __dcDccNanoControler_H__
#define __dcDccNanoControler_H__
//-------------------------------------------------------------------

// This file is just a link to the real include file...
// this is the way to keep

////////////////////////////////////////////////////////
// Add a '//' at the beginning of the line to be in 
// release mode.
//#define DDC_DEBUG_MODE

///////////////////////////////////////////////////////
// Verbose mode lets you see all actions done by the 
// library, but with a real flood of text to console...
// Has no effect if DEBUG_MODE is not activated.
//#define DEBUG_VERBOSE_MODE

#define GPIO2_PREFER_SPEED    1

#ifdef VISUALSTUDIO
#define DDC_DEBUG_MODE
#define ARDUINO_AVR_MEGA	// for test
#include "../../DIO2/VStudio/DIO2.h"
#else
#if !defined(ARDUINO_AVR_UNO) && !defined(ARDUINO_AVR_NANO)      // Must be compiled for UNO or NANO
#error DcDccNanoControler can only work for Arduino UNO or NANO !
#endif

#include "DIO2.h"
#endif

#ifndef __LcdUi_H__
#include "LcdUi.h"
#endif

#define FUNCTION_NUMBER		2

#ifdef DDC_DEBUG_MODE
static void CheckPinNb(GPIO_pin_t inPin, const __FlashStringHelper *inFunc);
static void CheckPinNb(int inPin, const __FlashStringHelper *inFunc);

#define CHECKPIN(val, text)		CheckPinNb(cval, text)
#else
#define CHECKPIN(val, text)
#endif

extern const char * const DDC_config_table[] PROGMEM;
extern const char * const string_table[] PROGMEM;

#if defined(__DDCLanguage_H__)

#ifdef SHORTLENGTH_MSG
const char str_title[] PROGMEM = "Dc/Dcc Nano V0.1";
const char str_copyright[] PROGMEM = "By Thierry Paris";
#else
const char str_title[] PROGMEM = "Dc/Dcc Nano Controler V0.1";
const char str_copyright[] PROGMEM = "By Thierry Paris";
#endif

const char * const DcDccStringTable[] PROGMEM
{
	str_title,
	str_copyright,
	str_dc,
	str_dcc,
	str_stop,
	str_stop2,
	str_modemodechoice,
	str_modelococtrl,
	str_modeconfig,
	str_yes,
	str_no,
	str_confirm,
	str_pwmfreqency,
	str_locoedit,
	str_locoId,
	str_locoName,
	str_locoSteps,
	str_locoSteps14,
	str_locoSteps28,
	str_locoSteps128,
	str_functions,
	str_dcslow,
	str_programcv
};
#endif

#define STR_TITLE			0
#define STR_COPYRIGHT		1
#define STR_DC				2
#define STR_DCC				3
#define STR_STOP			4
#define STR_STOP2			5
#define STR_MODEMODECHOICE	6
#define STR_MODELOCOCTRL	7
#define STR_MODECONFIG		8
#define STR_YES				9
#define STR_NO				10
#define STR_CONFIRM			11
#define STR_PWMFREQCFG		12
#define STR_LOCOEDIT		13
#define STR_LOCOID			14
#define STR_LOCONAME		15
#define STR_LOCOSTEPS		16
#define STR_LOCOSTEPS14		17
#define STR_LOCOSTEPS28		18
#define STR_LOCOSTEPS128	19
#define STR_FUNCTIONS		20
#define STR_DCSLOW			21
#define STR_MODIFYCV		22

//////////////////////////////////////////
//  Exclusion area
//
// You can exclude some parts of library here, to avoid losing program and data memory
// on parts you don t use.
// For example, if you don t want buttons commander
// just uncomment the line #define NO_BUTTONSCOMMANDER by removing // at the beginning.
//
// Arduino IDE build of list of the files to compile from the content of the library directory
// in a makefile and give it to Gcc. So Gcc compiles all the files of the directory
// even if its content is not used. The result is, if an unused source file contains
// static declarations, these statics will be allocated a this memory will be lost.
// The only solution I have found is to rename the source files to something IDE and Gcc don t know...
//
// So if you want to lose less memory, you can rename the linked files
// from .cpp to .cpp.ori, and from .hpp to .hpp.ori.
// See below the file names related to each exclusion:
//
//#define NO_SCREEN

/////////////////////////////////////

enum DcDcc
{
	DcChangeStopped,
	PanicStopped,
	ProgramMode,
	Dc,
	Dcc
};

#include "Locomotive.hpp"

#include "ControlerDc.hpp"
#include "ControlerDccpp.hpp"

#include "Handle.hpp"

// Copy from cmdrarduino ...
#define DCC_SHORT_ADDRESS           0x00
#define DCC_LONG_ADDRESS            0x01

// LcdUi defines
#define WINDOWTYPE_INTERRUPT_EMERGENCY	10
#define EEPROM_DDC_CONFIG_SIZE			64

#include <LcdEvents.h>

class DcDccControler
{
public:
	static DcDcc dcType;
	static DcDcc dcTypeAtStart;
	static Handle handle;
	//static int handleAddcounter;
	static Controler *pControler;

private:
	DcDccControler() {}

	static bool beginFinished;
	static void beforeFirstLoop();

public:
	static void begin(uint8_t inDcDccSelectPin, LcdScreen *apScreen);
	static void beginMain(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor);
	static void beginProg(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor);
	//static void AddHandle(Handle *pHandle);
	//static byte IndexOf(Handle *inpHandle);

	static void loop(unsigned long inEvent, int inData);

	static void ConfigLoad();
	static int ConfigSave();
	static void ConfigReset();

#ifdef DDC_DEBUG_MODE
	static void CheckIndex(unsigned char inIndex, const __FlashStringHelper *inFunc);
#endif
};
#endif

