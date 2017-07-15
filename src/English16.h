//-------------------------------------------------------------------
#ifndef __DDCLanguage_H__
#define __DDCLanguage_H__
//-------------------------------------------------------------------

#include "arduino.h"

// English language	for 16 characters display.

#define SHORTLENGTH_MSG
										//   0123456789012345
const char  str_dc[] PROGMEM =				"Dc";
const char  str_dcc[] PROGMEM=				"Dcc";
const char  str_stop[] PROGMEM=				"Stop Alim";
const char  str_stop2[] PROGMEM=			"Push cancel";
const char  str_modemodechoice[] PROGMEM =	"DDC menu:";
const char  str_modelococtrl[] PROGMEM =	"Control loco";
const char  str_modeconfig[] PROGMEM =		"Config DDC";
const char  str_yes[] PROGMEM =				"yes";
const char  str_no[] PROGMEM =				"no";
const char  str_confirm[] PROGMEM =			"Sure?";
const char  str_pwmfreqency[] PROGMEM =		"PWM Frequency";
const char  str_locoedit[] PROGMEM =		"Edit loco";
const char  str_locoId[] PROGMEM =			"Address";
const char  str_locoName[] PROGMEM =		"Name";
const char  str_locoSteps[] PROGMEM =		"Speed Steps";
const char  str_locoSteps14[] PROGMEM =		"14";
const char  str_locoSteps28[] PROGMEM =		"28";
const char  str_locoSteps128[] PROGMEM =	"128";
const char  str_functions[] PROGMEM =		"Functions";
const char  str_dcslow[] PROGMEM =			"Dc slow";
const char  str_programcv[] PROGMEM =		"Modify Cv";

// Extended part of messages (MEGA)

const char  str_resetconfig[] PROGMEM =		"Reset Config";
const char  str_engineShed[] PROGMEM =		"EngineShed";
const char  str_rollingStock[] PROGMEM =	"Rolling Stock";
const char  str_slotAdd[] PROGMEM =			"PowerUp";
const char  str_slotRemove[] PROGMEM =		"Garage";
const char  str_slotRemoveConfirm[] PROGMEM = "Garage...";
const char  str_slotMaintain[] PROGMEM =	"Maintain";
const char  str_locoAdd[] PROGMEM =			"Delivery";
const char  str_locoRemove[] PROGMEM =		"Deregister";
const char  str_locoRemoveConfirm[] PROGMEM ="Deregister...";
const char  str_back[] PROGMEM =			"Back";
const char  str_empty[] PROGMEM =			"Empty";
const char  str_workshop[] PROGMEM =		"Workshop";
const char  str_workshopMove[] PROGMEM =	"Move To";
const char  str_workshopBack[] PROGMEM =	"Back To Stock";
const char  str_workshopRepair[] PROGMEM =	"Repair";
										//   0123456789012345

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
																		 