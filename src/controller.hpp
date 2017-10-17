//-------------------------------------------------------------------
#ifndef __Controller_H__
#define __Controller_H__
//-------------------------------------------------------------------

class Controller
{
protected:
	Locomotive *pControlled;
	bool panicStopped;
	byte maxSpeed;

public:
	inline Controller() { this->pControlled = 0; this->panicStopped = false; this->maxSpeed = 128; }
		
	public:
		inline void SetControlled(Locomotive *inpControlled) { this->pControlled = inpControlled; }
		inline virtual void begin() {}
		inline virtual void beginMain(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor) {}
		inline virtual void beginProg(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor) {}
		inline virtual bool SetSpeed(int inNewSpeed) { return false; }
		inline virtual bool SetDirection(bool inToLeft) { return false; }
		inline virtual void SetFunction(byte inFunctionNumber, bool inActivate) { }
		inline virtual void SetAccessory(int inAddress, byte inSubAddress, bool inActivate) { }
		inline virtual void PanicStop(bool inStop) { }
		inline virtual void loop() { }

		inline byte GetMaxSpeed() { return this->maxSpeed; }
		inline bool IsPanicStopped() { return this->panicStopped; }
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
