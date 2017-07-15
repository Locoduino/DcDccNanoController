//-------------------------------------------------------------------
#ifndef __locomotive_H__
#define __locomotive_H__
//-------------------------------------------------------------------
#include "DcDccNanoControler.h"
#include "Function.hpp"

#ifndef FUNCTION_NUMBER
#error "FUNCTION_NUMBER should be defined"
#endif
//-------------------------------------------------------------------

#define DCC_SHORT_ADDRESS           0x00
#define DCC_LONG_ADDRESS            0x01

#define DCC_LOCONAME_LENGTH			12

#define LOCOMOTIVE_TYPE				'L'

class Locomotive
{
private:
	byte SlotNumber;	// Slot number in the EEPROM list, 255 (EEPROMLIST_EMPTY_SLOT) if not saved.
	int DccId;
	byte steps;

	int mappedSpeed;
	bool directionToLeft;

public:
	Function Functions[FUNCTION_NUMBER];
	static Locomotive AnalogLocomotive;

public:
	Locomotive();
	Locomotive(int inDccId, uint8_t adressKind, char *inName, uint8_t inSteps = 128);

	void ClearFunctions();
	byte GetFunctionIndex(const int inDccId) const;
	void Copy(Locomotive &inLocomotive);
	inline int GetDccId() const { return this->DccId; }
	inline void SetDccId(int inDccId) { this->DccId = inDccId; }
	inline int *GetDccIdAddress() { return &(this->DccId); }

	inline byte GetSteps() const { return this->steps; }
	inline void SetSteps(byte inSteps) { this->steps = inSteps; }
	inline byte *GetStepsAddress() { return &(this->steps); }

	inline int GetMappedSpeed() const { return  this->mappedSpeed; }
	inline bool GetDirectionToLeft() const { return  this->directionToLeft; }
	inline void SetMappedSpeed(int inSpeed) { this->mappedSpeed = inSpeed; }
	inline void SetDirectionToLeft(bool inDirectionToLeft) { this->directionToLeft = inDirectionToLeft; }
	inline byte GetSlotNumber() const { return this->SlotNumber; }
	inline void SetSlotNumber(byte inSlotNumber) { this->SlotNumber = inSlotNumber; }

	void Load(int inStartPos);
	void Save(int inStartPos);
	void Clear();

#ifdef DDC_DEBUG_MODE
	void CheckIndex(byte inIndex, const __FlashStringHelper *inFunc);
	void print();
#endif
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
