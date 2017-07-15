//-------------------------------------------------------------------
#ifndef __controlerDc_H__
#define __controlerDc_H__
//-------------------------------------------------------------------

#include "Controler.hpp"

//-------------------------------------------------------------------

#define SLOWMODELIMIT		100
#define BASE_PWM_FREQ_11_3	31250
#define NB_PWM_FREQ_11_3	7


class ControlerDc : public Controler
{
	private:
		uint8_t dcPWMpin;	// SignalEnable
		uint8_t dcDirPin;	// Signal
		uint8_t dcCurrentMonitor;
		bool slowMode;

	public:
		static const byte divisors_11_3_pow2[];
		byte DCFrequencyDivisorIndex;

		inline ControlerDc() { this->dcPWMpin = 0; this->dcDirPin = 0; this->DCFrequencyDivisorIndex = 3; this->slowMode = false; this->maxSpeed = 255; }
		
	public:
		void begin();
		void beginMain(uint8_t inDummy1, uint8_t SignalPin, uint8_t SignalEnablePin, uint8_t CurrentMonitor);
		bool SetSpeed(int inNewSpeed);
		bool SetDirection(bool inToLeft);
		void PanicStop(bool inStop);
		inline bool IsSlowMode() { return this->slowMode; }
		void SetSlowMode(bool inSlowMode);
		void SetFrequencyDivisorIndex(byte inDivisor);

		static void BuildFreqString(unsigned int inDivisor);
		static void BuildFreqIndexString(byte inIndexValue) { BuildFreqString(1 << divisors_11_3_pow2[inIndexValue]); }
		inline static unsigned int GetFrequencyDivisor(byte inIndexValue) { return 1 << divisors_11_3_pow2[inIndexValue]; }

	private:
		void SetFrequencyDivisorRaw(unsigned int inDivisor);
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
