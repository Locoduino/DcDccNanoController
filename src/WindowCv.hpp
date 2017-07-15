//-------------------------------------------------------------------
#ifndef __windowCv_H__
#define __windowCv_H__
//-------------------------------------------------------------------
//-------------------------------------------------------------------
#include "DcDccNanoControler.h"
//-------------------------------------------------------------------

#define WINDOWTYPE_CV	15

//-------------------------------------------------------------------

// Change an integer value

/// +----------------+
/// |Modify Cv       |
/// |Cv  0001 = >200<|
/// |                |
/// |                |
/// +----------------+

class WindowCv : public Window
{
protected:
	bool valueEdited;	// if false, the cv address itself is edited
	bool undefValue;	// True if tha value has not been read from the decoder...

public:
	int cvAddress;
	byte value;

	inline WindowCv() { this->valueEdited = false; this->value = 0; this->cvAddress = 1; }
	inline WindowCv(byte inFirstLine) { this->begin(inFirstLine); }

	void begin(byte inFirstLine);
	inline byte GetType() const { return WINDOWTYPE_CV; }

	void Event(byte inEventType, LcdUi *inpLcd);

#ifdef LCDUI_PRINT_WINDOWS
	void printWindow();
#endif
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
