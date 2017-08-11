//-------------------------------------------------------------------
#ifndef __windowLocoControl_H__
#define __windowLocoControl_H__
//-------------------------------------------------------------------
#include "DcDccNanocontroller.h"
//-------------------------------------------------------------------

#define WINDOWTYPE_LOCOCONTROL	14

class WindowLocoControl : public Window
{
private:
	Handle *pHandle;
	int dcMsg;
	int dccMsg;

public:
	inline WindowLocoControl() {}
	void begin(int inFirstLine, Handle *inpHandle);

	inline byte GetType() const { return WINDOWTYPE_LOCOCONTROL; }
	void Event(byte inEventType, LcdUi *inpLcd);
};

#endif
//-------------------------------------------------------------------
