/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Class for a choice of DC PWM frequency>
*************************************************************/

#include "DcDccNanoController.h"
#include "WindowChooseDcFreq.hpp"

byte WindowChooseDcFreq::GetChoiceTextNumber() const
{
	return NB_PWM_FREQ_11_3;
}

char *WindowChooseDcFreq::GetChoiceTextValue(byte indexValue, LcdScreen *apScreen) const
{
	ControllerDc::BuildFreqIndexString(indexValue);

	return LcdScreen::buffer;
}

/*void WindowChooseDcFreq::SetCurrentChoice(unsigned int inDivisor)
{
	BuildFreqString(inDivisor);
	WindowChoiceText::SetCurrentChoice(LcdScreen::buffer, inDivisor);
} */

void WindowChooseDcFreq::move(bool inMore)
{
	int i = *(this->pValue);
	if (inMore)
	{
		i++;
		if (ControllerDc::divisors_11_3_pow2[i] == 255)
			i = NB_PWM_FREQ_11_3 - 1;
	}
	else
	{
		if (i > 0)
			i--;
	}

	if (i >= 99)
		i = 0;
	SetCurrentChoice(i);
}

