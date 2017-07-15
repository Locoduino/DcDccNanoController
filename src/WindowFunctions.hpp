//-------------------------------------------------------------------
#ifndef __windowFunctions_H__
#define __windowFunctions_H__
//-------------------------------------------------------------------
//-------------------------------------------------------------------
#include "DcDccNanoControler.h"
//-------------------------------------------------------------------

#define WINDOWTYPE_FUNCTIONS	16

//-------------------------------------------------------------------

// Change the id of a function

/// +----------------+
/// |Modify Function |
/// |F    1  =  >01< |
/// |                |
/// |                |
/// +----------------+

class WindowFunctions : public Window
{
protected:
	bool valueEdited;	// if false, the cv address itself is edited
	bool undefValue;	// True if tha value has not been read from the decoder...

public:
	int fAddress;
	byte value;

	inline WindowFunctions() { this->valueEdited = false; this->value = 0; this->fAddress = 0; }
	inline WindowFunctions(byte inFirstLine) { this->begin(inFirstLine); }

	void begin(byte inFirstLine);
	inline byte GetType() const { return WINDOWTYPE_FUNCTIONS; }

	void Event(byte inEventType, LcdUi *inpLcd);

#ifdef LCDUI_PRINT_WINDOWS
	void printWindow();
#endif
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
