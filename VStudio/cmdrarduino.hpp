//-------------------------------------------------------------------
#ifndef __cmdrarduino_H__
#define __cmdrarduino_H__
//-------------------------------------------------------------------

#define DCC_SHORT_ADDRESS           0x00
#define DCC_LONG_ADDRESS            0x01

class DCCPacketScheduler
{
public:
	void setup(void) {}

	//for enqueueing packets
	bool setSpeed(uint16_t address, uint8_t address_kind, int8_t new_speed, uint8_t steps = 0) { return false; } //new_speed: [-127,127]
	bool setSpeed14(uint16_t address, uint8_t address_kind, int8_t new_speed, bool F0 = true) { return false; } //new_speed: [-13,13], and optionally F0 settings.
	bool setSpeed28(uint16_t address, uint8_t address_kind, int8_t new_speed) { return false; } //new_speed: [-28,28]
	bool setSpeed128(uint16_t address, uint8_t address_kind, int8_t new_speed) { return false; } //new_speed: [-127,127]

	//the function methods are NOT stateful; you must specify all functions each time you call one
	//keeping track of function state is the responsibility of the calling program.
	bool setFunctions(uint16_t address, uint8_t address_kind, uint8_t F0to4, uint8_t F5to9 = 0x00, uint8_t F9to12 = 0x00) { return false; }
	bool setFunctions(uint16_t address, uint8_t address_kind, uint16_t functions) { return false; }
	bool setFunctions0to4(uint16_t address, uint8_t address_kind, uint8_t functions) { return false; }
	bool setFunctions5to8(uint16_t address, uint8_t address_kind, uint8_t functions) { return false; }
	bool setFunctions9to12(uint16_t address, uint8_t address_kind, uint8_t functions) { return false; }
	//other cool functions to follow. Just get these working first, I think.

	bool opsAddressOnly(uint8_t CV1_data) { return false; }
	bool opsProgDirectCV(uint16_t CV, uint8_t CV_data) { return false; }
	bool opsProgramCV(uint16_t address, uint8_t address_kind, uint16_t CV, uint8_t CV_data) { return false; }
	void opsEndProgram(void) {} 

	//more specific functions
	bool eStop(void) { return false; } //all locos
	bool eStop(uint16_t address, uint8_t address_kind) { return false; } //just one specific loco

	//to be called periodically within loop()
	void update(void) {} //checks queues, puts whatever's pending on the rails via global current_packet. easy-peasy
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------