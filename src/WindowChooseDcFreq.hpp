//-------------------------------------------------------------------
#ifndef __windowChooseDcFreq_H__
#define __windowChooseDcFreq_H__
//-------------------------------------------------------------------

#include "DcDccNanoController.h"

//-------------------------------------------------------------------

#define WINDOWTYPE_FREQCHOICE	12

/*
Choose a frequency for the DC PWM output.
*/

class WindowChooseDcFreq : public WindowChoiceText
{
public:
	WindowChooseDcFreq() {}
	void begin(int inFirstLine, byte *inpFreq) { WindowChoiceText::begin(inFirstLine, inpFreq); }

	byte GetChoiceTextNumber() const;
	char *GetChoiceTextValue(byte indexValue, LcdScreen *apScreen) const;

	inline void BuildFreqString() { return ControllerDc::BuildFreqIndexString(*(this->pValue)); }
	inline byte GetType() const { return WINDOWTYPE_FREQCHOICE; }
	inline void MoveNextChoice() { move(true); }
	inline void MovePreviousChoice() {	move(false); }
//	inline unsigned int GetFrequencyDivisor() const { return GetFrequencyDivisor(*(this->pValue)); }

private:
	void move(bool inMore);
};
#endif
//-------------------------------------------------------------------
