//-------------------------------------------------------------------
#ifndef __function_H__
#define __function_H__
//-------------------------------------------------------------------

#define FUNCTION_TYPE				'F'

class Function
{
	public:
		byte SlotNumber;	// Slot number in the EEPROM list, 255 (EEPROMLIST_EMPTY_SLOT) if not saved.
		int DccIdFunction;

	private:
		bool activated;

	public:
		Function() { this->Clear(); }
		Function(int DccId);
		inline Function(const Function &inFunction) { this->Copy(inFunction); }

		void Copy(const Function &inFunction);
		void Clear();
		inline byte GetSlotNumber() const { return this->SlotNumber; }
		inline void SetSlotNumber(byte inSlotNumber) { this->SlotNumber = inSlotNumber; }

		inline uint16_t GetDccId() const { return this->DccIdFunction; }
		inline void SetDccId(uint16_t inDccId) { this->DccIdFunction = inDccId; }

		inline bool IsActivated() const { return this->activated; }
		inline void SetActivated(bool inActivated) { this->activated = inActivated; }
		inline bool Toggle() { this->activated = !this->activated;  return this->activated; }

		void Load(int inStartPos);
		void Save(int inStartPos);
};

//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
