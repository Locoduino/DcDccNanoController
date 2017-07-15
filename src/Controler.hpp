//-------------------------------------------------------------------
#ifndef __controler_H__
#define __controler_H__
//-------------------------------------------------------------------

class Controler
{
protected:
	Locomotive *pControled;
	bool panicStopped;
	byte maxSpeed;

public:
	inline Controler() { this->pControled = 0; this->panicStopped = false; this->maxSpeed = 128; }
		
	public:
		inline void SetControled(Locomotive *inpControled) { this->pControled = inpControled; }
		inline virtual void begin() {}
		inline virtual void beginMain(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor) {}
		inline virtual void beginProg(uint8_t DirectionMotor, uint8_t DccSignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor) {}
		inline virtual bool SetSpeed(int inNewSpeed) { return false; }
		inline virtual bool SetDirection(bool inToLeft) { return false; }
		inline virtual void SetFunction(byte inFunctionNumber, bool inActivate) { }
		inline virtual void PanicStop(bool inStop) { }
		inline virtual void loop() { }

		inline byte GetMaxSpeed() { return this->maxSpeed; }
		inline bool IsPanicStopped() { return this->panicStopped; }
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
