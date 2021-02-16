/**********************************************************************

PacketRegister.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman
Adapted for DcDcc by Thierry PARIS

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "../DcDccNanoController.h"
//#include "DCCpp_Uno.h"
//#include "PacketRegister.h"
//#include "Comm.h"

byte DccppConfig::SignalEnablePinMain = 255;
byte DccppConfig::CurrentMonitorMain = 255;
					
byte DccppConfig::SignalEnablePinProg = 255;
byte DccppConfig::CurrentMonitorProg = 255;
					
byte DccppConfig::DirectionMotorA = 255;
byte DccppConfig::DirectionMotorB = 255;

///////////////////////////////////////////////////////////////////////////////

void Register::initPackets(){
  activePacket=packet;
  updatePacket=packet+1;
} // Register::initPackets

///////////////////////////////////////////////////////////////////////////////
    
RegisterList::RegisterList(int maxNumRegs){
  this->maxNumRegs=maxNumRegs;
  reg=(Register *)calloc((maxNumRegs+1),sizeof(Register));
  for(int i=0;i<=maxNumRegs;i++)
    reg[i].initPackets();
  regMap=(Register **)calloc((maxNumRegs+1),sizeof(Register *));
  speedTable=(int *)calloc((maxNumRegs+1),sizeof(int *));
  currentReg=reg;
  regMap[0]=reg;
  maxLoadedReg=reg;
  nextReg=NULL;
  currentBit=0;
  nRepeat=0;
} // RegisterList::RegisterList
  
///////////////////////////////////////////////////////////////////////////////

// LOAD DCC PACKET INTO TEMPORARY REGISTER 0, OR PERMANENT REGISTERS 1 THROUGH DCC_PACKET_QUEUE_MAX (INCLUSIVE)
// CONVERTS 2, 3, 4, OR 5 BYTES INTO A DCC BIT STREAM WITH PREAMBLE, CHECKSUM, AND PROPER BYTE SEPARATORS
// BITSTREAM IS STORED IN UP TO A 10-BYTE ARRAY (USING AT MOST 76 OF 80 BITS)

void RegisterList::loadPacket(int nReg, byte *b, int nBytes, int nRepeat, int printFlag) volatile 
{  
#ifdef VISUALSTUDIO
  return;
#endif
  nReg=nReg%((maxNumRegs+1));          // force nReg to be between 0 and maxNumRegs, inclusive

  while(nextReg!=NULL);              // pause while there is a Register already waiting to be updated -- nextReg will be reset to NULL by interrupt when prior Register updated fully processed
 
  if(regMap[nReg]==NULL)              // first time this Register Number has been called
   regMap[nReg]=maxLoadedReg+1;       // set Register Pointer for this Register Number to next available Register
 
  Register *r=regMap[nReg];           // set Register to be updated
  Packet *p=r->updatePacket;          // set Packet in the Register to be updated
  byte *buf=p->buf;                   // set byte buffer in the Packet to be updated
          
  b[nBytes]=b[0];                        // copy first byte into what will become the checksum byte  
  for(int i=1;i<nBytes;i++)              // XOR remaining bytes into checksum byte
    b[nBytes]^=b[i];
  nBytes++;                              // increment number of bytes in packet to include checksum byte
      
  buf[0]=0xFF;                        // first 8 bytes of 22-byte preamble
  buf[1]=0xFF;                        // second 8 bytes of 22-byte preamble
  buf[2]=0xFC + bitRead(b[0],7);      // last 6 bytes of 22-byte preamble + data start bit + b[0], bit 7
  buf[3]=b[0]<<1;                     // b[0], bits 6-0 + data start bit
  buf[4]=b[1];                        // b[1], all bits
  buf[5]=b[2]>>1;                     // b[2], bits 7-1
  buf[6]=b[2]<<7;                     // b[2], bit 0
  
  if(nBytes==3){
    p->nBits=49;
  } else{
    buf[6]+=b[3]>>2;                  // b[3], bits 7-2
    buf[7]=b[3]<<6;                   // b[3], bit 1-0
    if(nBytes==4){
      p->nBits=58;
    } else{
      buf[7]+=b[4]>>3;                // b[4], bits 7-3
      buf[8]=b[4]<<5;                 // b[4], bits 2-0
      if(nBytes==5){
        p->nBits=67;
      } else{
        buf[8]+=b[5]>>4;              // b[5], bits 7-4
        buf[9]=b[5]<<4;               // b[5], bits 3-0
        p->nBits=76;
      } // >5 bytes
    } // >4 bytes
  } // >3 bytes
  
  nextReg=r;
  this->nRepeat=nRepeat;
  maxLoadedReg=max(maxLoadedReg,nextReg);
  
#ifdef DDC_DEBUG_MODE
  if(printFlag && SHOW_PACKETS)       // for debugging purposes
    printPacket(nReg,b,nBytes,nRepeat);  
#endif

} // RegisterList::loadPacket

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setThrottle(int nReg, int cab, int tSpeed, int tDirection) volatile 
{
	byte b[5];                      // save space for checksum byte
	byte nB = 0;

	if (nReg<1 || nReg>maxNumRegs)
		return;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

	b[nB++] = lowByte(cab);
	b[nB++] = 0x3F;                        // 128-step speed control byte
	if (tSpeed >= 0)
		b[nB++] = tSpeed + (tSpeed>0) + tDirection * 128;   // max speed is 126, but speed codes range from 2-127 (0=stop, 1=emergency stop)
	else {
		b[nB++] = 1;
		tSpeed = 0;
	}

	loadPacket(nReg, b, nB, 0, 1);

#ifdef DDC_DEBUG_MODE
	INTERFACE.print("<T");
	INTERFACE.print(nReg); INTERFACE.print(" ");
	INTERFACE.print(tSpeed); INTERFACE.print(" ");
	INTERFACE.print(tDirection);
	INTERFACE.print(">");
#endif
	speedTable[nReg] = tDirection == 1 ? tSpeed : -tSpeed;

} // RegisterList::setThrottle(ints)

///////////////////////////////////////////////////////////////////////////////

void RegisterList::setFunction(int nReg, int cab, int fByte, int eByte) volatile
{
	byte b[5];                      // save space for checksum byte
	byte nB = 0;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

	b[nB++] = lowByte(cab);

	if (eByte < 0) {                      // this is a request for functions FL,F1-F12  
		b[nB++] = (fByte | 0x80) & 0xBF;     // for safety this guarantees that first nibble of function byte will always be of binary form 10XX which should always be the case for FL,F1-F12  
	}
	else {                             // this is a request for functions F13-F28
		b[nB++] = (fByte | 0xDE) & 0xDF;     // for safety this guarantees that first byte will either be 0xDE (for F13-F20) or 0xDF (for F21-F28)
		b[nB++] = eByte;
	}

	loadPacket(nReg, b, nB, 4, 1);

} // RegisterList::setFunction(ints)

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::setAccessory(int aAdd, int aNum, int activate) volatile 
{
	byte b[3];                      // save space for checksum byte

	b[0] = aAdd % 64 + 128;                                           // first byte is of the form 10AAAAAA, where AAAAAA represent 6 least signifcant bits of accessory address  
	b[1] = ((((aAdd / 64) % 8) << 4) + (aNum % 4 << 1) + activate % 2) ^ 0xF8;      // second byte is of the form 1AAACDDD, where C should be 1, and the least significant D represent activate/deactivate

	loadPacket(0, b, 2, 4, 1);

} // RegisterList::setAccessory(ints)

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeTextPacket(int nReg, byte *b, int nBytes) volatile 
{

	if (nBytes<2 || nBytes>5) {    // invalid valid packet
		INTERFACE.print("<mInvalid Packet>");
		return;
	}

	loadPacket(nReg, b, nBytes, 0, 1);

} // RegisterList::writeTextPacket(bytes)

  ///////////////////////////////////////////////////////////////////////////////

int RegisterList::buildBaseAcknowlegde(int inMonitorPin) volatile
{
	int base = 0;
	for (int j = 0; j < ACK_BASE_COUNT; j++)
	{
		int val = (int)analogRead(inMonitorPin);
		base += val;
	}

	return base / ACK_BASE_COUNT;
}

bool RegisterList::checkAcknowlegde(int inMonitorPin, int inBase) volatile
{
	int c = 0;
	int max = 0;

#if defined(ARDUINO_ARCH_ESP32)
	int loopMax = 20;
#else
	int loopMax = 1;
#endif

#ifdef DCCPP_DEBUG_MODE
	int loop = 0;
#endif
	for (int a = 0; a < loopMax; a++)
	{
		c = 0;
		for (int j = 0; j < ACK_SAMPLE_COUNT; j++)
		{
			int val = (int)analogRead(inMonitorPin);
			c = (int)((val - inBase) * ACK_SAMPLE_SMOOTHING + c * (1.0 - ACK_SAMPLE_SMOOTHING));
			if (c > max)
			{
				max = c;
#ifdef DCCPP_DEBUG_MODE
				loop = a;
#endif
			}
		}
	}

#ifdef DCCPP_DEBUG_MODE
	Serial.print(F(" iter : "));
	Serial.print(loop);
	Serial.print(", max : ");
	Serial.println(max);
#endif

	return (max > ACK_SAMPLE_THRESHOLD);
}

int RegisterList::readCVraw(int cv, int callBack, int callBackSub, bool FromProg) volatile
{
	byte bRead[4];
	int bValue;
	bool ret;
	int base;
	cv--;                              // actual CV addresses are cv-1 (0-1023)

	byte MonitorPin = DccppConfig::CurrentMonitorProg;
	if (!FromProg)
		MonitorPin = DccppConfig::CurrentMonitorMain;

	// A read cannot be done if a monitor pin is not defined !
	if (MonitorPin == 255)
		return -1;

	bRead[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bRead[1] = lowByte(cv);

	bValue = 0;

	for (int i = 0; i<8; i++) {

		base = RegisterList::buildBaseAcknowlegde(MonitorPin);

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

		bRead[2] = 0xE8 + i;

		loadPacket(0, resetPacket, 2, 3);          // NMRA recommends starting with 3 reset packets
		loadPacket(0, bRead, 3, 5);                // NMRA recommends 5 verify packets
//		loadPacket(0, resetPacket, 2, 1);          // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
		loadPacket(0, idlePacket, 2, 6);          // NMRA recommends 6 idle or reset packets for decoder recovery time

#if defined(ARDUINO_ARCH_ESP32)
		delay(2);
#endif

		ret = RegisterList::checkAcknowlegde(MonitorPin, base);

		bitWrite(bValue, i, ret);
	}

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

	base = RegisterList::buildBaseAcknowlegde(MonitorPin);

#if defined(ARDUINO_ARCH_ESP32)
		delay(10);
#endif

	bRead[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte
	bRead[2] = bValue;

	loadPacket(0, resetPacket, 2, 3);       // NMRA recommends starting with 3 reset packets
	loadPacket(0, bRead, 3, 5);             // NMRA recommends 5 verify packets
	//loadPacket(0, resetPacket, 2, 1);     // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
	loadPacket(0, idlePacket, 2, 6);				// NMRA recommends 6 idle or reset packets for decoder recovery time

#if defined(ARDUINO_ARCH_ESP32)
		delay(2);
#endif

	ret = RegisterList::checkAcknowlegde(MonitorPin, base);

	if (ret == false)    // verify unsuccessful
		bValue = -1;

#ifdef DDC_DEBUG_MODE
	INTERFACE.print("<r");
	INTERFACE.print(callBack);
	INTERFACE.print("|");
	INTERFACE.print(callBackSub);
	INTERFACE.print("|");
	INTERFACE.print(cv + 1);
	INTERFACE.print(" ");
	INTERFACE.print(bValue);
	INTERFACE.print(">");
#endif

	return bValue;
}

void RegisterList::readCV(int cv, int callBack, int callBackSub) volatile 
{
	/*int bValue = */RegisterList::readCVraw(cv, callBack, callBackSub, true);

} // RegisterList::readCV(ints)

int RegisterList::readCVmain(int cv, int callBack, int callBackSub) volatile
{
	return RegisterList::readCVraw(cv, callBack, callBackSub, false);

} // RegisterList::readCV_Main()

 ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVByte(int cv, int bValue, int callBack, int callBackSub) volatile 
{
	byte bWrite[4];
	bool ok = false;
	int ret, base;

	cv--;                              // actual CV addresses are cv-1 (0-1023)

	bWrite[0] = 0x7C + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bWrite[1] = lowByte(cv);
	bWrite[2] = bValue;

	loadPacket(0, resetPacket, 2, 3);        // NMRA recommends starting with 3 reset packets
	loadPacket(0, bWrite, 3, 5);             // NMRA recommends 5 verify packets
	loadPacket(0, bWrite, 3, 6);             // NMRA recommends 6 write or reset packets for decoder recovery time

	/*loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, bWrite, 3, 4);
	loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, idlePacket, 2, 10);*/

	// If monitor pin undefined, write cv without any confirmation...
	if (DccppConfig::CurrentMonitorProg != 255)
	{
		base = RegisterList::buildBaseAcknowlegde(DccppConfig::CurrentMonitorProg);

		bWrite[0] = 0x74 + (highByte(cv) & 0x03);   // set-up to re-verify entire byte

		loadPacket(0, resetPacket, 2, 3);          // NMRA recommends starting with 3 reset packets
		loadPacket(0, bWrite, 3, 5);               // NMRA recommends 5 verify packets
		//loadPacket(0, resetPacket, 2, 1);          // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
		loadPacket(0, bWrite, 3, 6);               // NMRA recommends 6 write or reset packets for decoder recovery time

		ret = RegisterList::checkAcknowlegde(DccppConfig::CurrentMonitorProg, base);

		loadPacket(0, resetPacket, 2, 1);        // Final reset packet (and decoder begins to respond)

		if (ret == 0)    // verify unsuccessful
			ok = true;

#ifdef DDC_DEBUG_MODE
		INTERFACE.print("<r");
		INTERFACE.print(callBack);
		INTERFACE.print("|");
		INTERFACE.print(callBackSub);
		INTERFACE.print("|");
		INTERFACE.print(cv + 1);
		INTERFACE.print(" ");
		INTERFACE.print(bValue);
		INTERFACE.print(">");
#endif
	}
} // RegisterList::writeCVByte(ints)

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVBit(int cv, int bNum, int bValue, int callBack, int callBackSub) volatile 
{
	byte bWrite[4];
	int ret, base;

	cv--;                              // actual CV addresses are cv-1 (0-1023)
	bValue = bValue % 2;
	bNum = bNum % 8;

	bWrite[0] = 0x78 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	bWrite[1] = lowByte(cv);
	bWrite[2] = 0xF0 + bValue * 8 + bNum;

	/*loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, bWrite, 3, 4);
	loadPacket(0, resetPacket, 2, 1);
	loadPacket(0, idlePacket, 2, 10);*/

	loadPacket(0, resetPacket, 2, 3);        // NMRA recommends starting with 3 reset packets
	loadPacket(0, bWrite, 3, 5);             // NMRA recommends 5 verify packets
	loadPacket(0, bWrite, 3, 6);             // NMRA recommends 6 write or reset packets for decoder recovery time

	// If monitor pin undefined, write cv without any confirmation...
	if (DccppConfig::CurrentMonitorProg != 255)
	{
		base = RegisterList::buildBaseAcknowlegde(DccppConfig::CurrentMonitorProg);

		bitClear(bWrite[2], 4);              // change instruction code from Write Bit to Verify Bit

		loadPacket(0, resetPacket, 2, 3);          // NMRA recommends starting with 3 reset packets
		loadPacket(0, bWrite, 3, 5);               // NMRA recommends 5 verfy packets
		//loadPacket(0, resetPacket, 2, 1);          // forces code to wait until all repeats of bRead are completed (and decoder begins to respond)
		loadPacket(0, bWrite, 3, 6);           // NMRA recommends 6 write or reset packets for decoder recovery time

		ret = RegisterList::checkAcknowlegde(DccppConfig::CurrentMonitorProg, base);

#ifdef DDC_DEBUG_MODE
		INTERFACE.print("<r");
		INTERFACE.print(callBack);
		INTERFACE.print("|");
		INTERFACE.print(callBackSub);
		INTERFACE.print("|");
		INTERFACE.print(cv + 1);
		INTERFACE.print(" ");
		INTERFACE.print(bNum);
		INTERFACE.print(" ");
		INTERFACE.print(bValue);
		INTERFACE.print(">");
#endif
	}
} // RegisterList::writeCVBit(ints)

///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVByteMain(int cab, int cv, int bValue) volatile 
{
	byte b[6];                      // save space for checksum byte
	byte nB = 0;

	cv--;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

	b[nB++] = lowByte(cab);
	b[nB++] = 0xEC + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	b[nB++] = lowByte(cv);
	b[nB++] = bValue;

	loadPacket(0, b, nB, 4);

} // RegisterList::writeCVByteMain(ints)

  ///////////////////////////////////////////////////////////////////////////////

void RegisterList::writeCVBitMain(int cab, int cv, int bNum, int bValue) volatile 
{
	byte b[6];                      // save space for checksum byte
	byte nB = 0;

	cv--;

	bValue = bValue % 2;
	bNum = bNum % 8;

	if (cab>127)
		b[nB++] = highByte(cab) | 0xC0;      // convert train number into a two-byte address

	b[nB++] = lowByte(cab);
	b[nB++] = 0xE8 + (highByte(cv) & 0x03);   // any CV>1023 will become modulus(1024) due to bit-mask of 0x03
	b[nB++] = lowByte(cv);
	b[nB++] = 0xF0 + bValue * 8 + bNum;

	loadPacket(0, b, nB, 4);

} // RegisterList::writeCVBitMain(ints)

///////////////////////////////////////////////////////////////////////////////

#ifdef DDC_DEBUG_MODE
void RegisterList::printPacket(int nReg, byte *b, int nBytes, int nRepeat) volatile 
{
  
  INTERFACE.print("<*");
  INTERFACE.print(nReg);
  INTERFACE.print(":");
  for(int i=0;i<nBytes;i++){
    INTERFACE.print(" ");
    INTERFACE.print(b[i],HEX);
  }
  INTERFACE.print(" / ");
  INTERFACE.print(nRepeat);
  INTERFACE.println(">");
} // RegisterList::printPacket()
#endif

///////////////////////////////////////////////////////////////////////////////

byte RegisterList::idlePacket[3]={0xFF,0x00,0};                 // always leave extra byte for checksum computation
byte RegisterList::resetPacket[3]={0x00,0x00,0};

byte RegisterList::bitMask[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};         // masks used in interrupt routine to speed the query of a single bit in a Packet
