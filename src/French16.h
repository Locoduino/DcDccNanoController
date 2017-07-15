//-------------------------------------------------------------------
#ifndef __DDCLanguage_H__
#define __DDCLanguage_H__
//-------------------------------------------------------------------

#include "arduino.h"

// French language for 16 characters display.

#define SHORTLENGTH_MSG	
										  // 0123456789012345
const char  str_dc[] PROGMEM=				"Dc";
const char  str_dcc[] PROGMEM=				"Dcc";
const char  str_stop[] PROGMEM =			"Arret general";
const char  str_stop2[] PROGMEM =			"Appuyer annul";
const char  str_modemodechoice[] PROGMEM =	"DDC menu:";
const char  str_modelococtrl[] PROGMEM =	"Controler loco";
const char  str_modeconfig[] PROGMEM =		"Config DDC";
const char  str_yes[] PROGMEM =				"oui";
const char  str_no[] PROGMEM =				"non";
const char  str_confirm[] PROGMEM =			"Sur ?";
const char  str_pwmfreqency[] PROGMEM =		"Dc frequence";
const char  str_locoedit[] PROGMEM =		"Editer loco";
const char  str_locoId[] PROGMEM =			"Adresse";
const char  str_locoName[] PROGMEM =		"Nom";
const char  str_locoSteps[] PROGMEM =		"Nb Pas vitesse";
const char  str_locoSteps14[] PROGMEM =		"14";
const char  str_locoSteps28[] PROGMEM =		"28";
const char  str_locoSteps128[] PROGMEM =	"128";
const char  str_functions[] PROGMEM =		"Modif Fonctions";
const char  str_dcslow[] PROGMEM =			"Dc lent";
const char  str_programcv[] PROGMEM =		"Modifier Cv";

// Extended part of messages (MEGA)

const char  str_resetconfig[] PROGMEM =		"Reset Config";
const char  str_engineShed[] PROGMEM =		"Dépôt";
const char  str_rollingStock[] PROGMEM =	"Parc roulant";
const char  str_slotAdd[] PROGMEM =			"Démarrer";
const char  str_slotRemove[] PROGMEM =		"Garer";
const char  str_slotRemoveConfirm[] PROGMEM ="Garer...";
const char  str_slotMaintain[] PROGMEM =	"Maintenir";
const char  str_locoAdd[] PROGMEM =			"Livraison";
const char  str_locoRemove[] PROGMEM =		"Radiation";
const char  str_locoRemoveConfirm[] PROGMEM ="Radier...";
const char  str_back[] PROGMEM =			"reculer";
const char  str_empty[] PROGMEM =			"vide";
const char  str_workshop[] PROGMEM =		"Atelier";
const char  str_workshopMove[] PROGMEM =	"Aller";
const char  str_workshopBack[] PROGMEM =	"Retour";
const char  str_workshopRepair[] PROGMEM =	"Reparer";
										//   0123456789012345

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
