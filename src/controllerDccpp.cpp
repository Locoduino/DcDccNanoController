/*************************************************************
project: <Dc/Dcc Controller>
author: <Thierry PARIS>
description: <Dcc++ Controller>
*************************************************************/

/**********************************************************************

DCC++ BASE STATION
COPYRIGHT (c) 2013-2016 Gregg E. Berman

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses

**********************************************************************/
/**********************************************************************

DCC++ BASE STATION is a C++ program written for the Arduino Uno and Arduino Mega
using the Arduino IDE 1.6.6.

It allows a standard Arduino Uno or Mega with an Arduino Motor Shield (as well as others)
to be used as a fully-functioning digital command and control (DCC) base station
for controlling model train layouts that conform to current National Model
Railroad Association (NMRA) DCC standards.

This version of DCC++ BASE STATION supports:

* 2-byte and 4-byte locomotive addressing
* Simultaneous control of multiple locomotives
* 128-step speed throttling
* Cab functions F0-F28
* Activate/de-activate accessory functions using 512 addresses, each with 4 sub-addresses
- includes optional functionailty to monitor and store of the direction of any connected turnouts
* Programming on the Main Operations Track
- write configuration variable bytes
- set/clear specific configuration variable bits
* Programming on the Programming Track
- write configuration variable bytes
- set/clear specific configuration variable bits
- read configuration variable bytes

DCC++ BASE STATION is controlled with simple text commands received via
the Arduino's serial interface.  Users can type these commands directly
into the Arduino IDE Serial Monitor, or can send such commands from another
device or computer program.

When compiled for the Arduino Mega, an Ethernet Shield can be used for network
communications instead of using serial communications.

DCC++ CONTROLLER, available separately under a similar open-source
license, is a Java program written using the Processing library and Processing IDE
that provides a complete and configurable graphic interface to control model train layouts
via the DCC++ BASE STATION.

With the exception of a standard 15V power supply that can be purchased in
any electronics store, no additional hardware is required.

Neither DCC++ BASE STATION nor DCC++ CONTROLLER use any known proprietary or
commercial hardware, software, interfaces, specifications, or methods related
to the control of model trains using NMRA DCC standards.  Both programs are wholly
original, developed by the author, and are not derived from any known commercial,
free, or open-source model railroad control packages by any other parties.

However, DCC++ BASE STATION and DCC++ CONTROLLER do heavily rely on the IDEs and
embedded libraries associated with Arduino and Processing.  Tremendous thanks to those
responsible for these terrific open-source initiatives that enable programs like
DCC++ to be developed and distributed in the same fashion.

REFERENCES:

NMRA DCC Standards:          http://www.nmra.org/index-nmra-standards-and-recommended-practices
Arduino:                     http://www.arduino.cc/
Processing:                  http://processing.org/
GNU General Public License:  http://opensource.org/licenses/GPL-3.0

BRIEF NOTES ON THE THEORY AND OPERATION OF DCC++ BASE STATION:

DCC++ BASE STATION for the Uno configures the OC0B interrupt pin associated with Timer 0,
and the OC1B interupt pin associated with Timer 1, to generate separate 0-5V
unipolar signals that each properly encode zero and one bits conforming with
DCC timing standards.  When compiled for the Mega, DCC++ BASE STATION uses OC3B instead of OC0B.

Series of DCC bit streams are bundled into Packets that each form the basis of
a standard DCC instruction.  Packets are stored in Packet Registers that contain
methods for updating and queuing according to text commands sent by the user
(or another program) over the serial interface.  There is one set of registers that controls
the main operations track and one that controls the programming track.

For the main operations track, packets to store cab throttle settings are stored in
registers numbered 1 through MAX_MAIN_REGISTERS (as defined in DCCpp_Uno.h).
It is generally considered good practice to continuously send throttle control packets
to every cab so that if an engine should momentarily lose electrical connectivity with the tracks,
it will very quickly receive another throttle control signal as soon as connectivity is
restored (such as when a train passes over rough portion of track or the frog of a turnout).

DCC++ Base Station therefore sequentially loops through each main operations track packet register
that has been loaded with a throttle control setting for a given cab.  For each register, it
transmits the appropriate DCC packet bits to the track, then moves onto the next register
without any pausing to ensure continuous bi-polar power is being provided to the tracks.
Updates to the throttle setting stored in any given packet register are done in a double-buffered
fashion and the sequencer is pointed to that register immediately after being changes so that updated DCC bits
can be transmitted to the appropriate cab without delay or any interruption in the bi-polar power signal.
The cabs identified in each stored throttle setting should be unique across registers.  If two registers
contain throttle setting for the same cab, the throttle in the engine will oscillate between the two,
which is probably not a desireable outcome.

For both the main operations track and the programming track there is also a special packet register with id=0
that is used to store all other DCC packets that do not require continious transmittal to the tracks.
This includes DCC packets to control decoder functions, set accessory decoders, and read and write Configuration Variables.
It is common practice that transmittal of these one-time packets is usually repeated a few times to ensure
proper receipt by the receiving decoder.  DCC decoders are designed to listen for repeats of the same packet
and provided there are no other packets received in between the repeats, the DCC decoder will not repeat the action itself.
Some DCC decoders actually require receipt of sequential multiple identical one-time packets as a way of
verifying proper transmittal before acting on the instructions contained in those packets

An Arduino Motor Shield (or similar), powered by a standard 15V DC power supply and attached
on top of the Arduino Uno or Mega, is used to transform the 0-5V DCC logic signals
produced by the Uno's Timer interrupts into proper 0-15V bi-polar DCC signals.

This is accomplished on the Uno by using one small jumper wire to connect the Uno's OC1B output (pin 10)
to the Motor Shield's DIRECTION A input (pin 12), and another small jumper wire to connect
the Uno's OC0B output (pin 5) to the Motor Shield's DIRECTION B input (pin 13).

For the Mega, the OC1B output is produced directly on pin 12, so no jumper is needed to connect to the
Motor Shield's DIRECTION A input.  However, one small jumper wire is needed to connect the Mega's OC3B output (pin 2)
to the Motor Shield's DIRECTION B input (pin 13).

Other Motor Shields may require different sets of jumper or configurations (see Config.h and DCCpp_Uno.h for details).

When configured as such, the CHANNEL A and CHANNEL B outputs of the Motor Shield may be
connected directly to the tracks.  This software assumes CHANNEL A is connected
to the Main Operations Track, and CHANNEL B is connected to the Programming Track.

DCC++ BASE STATION in split into multiple modules, each with its own header file:

DCCpp_Uno:        declares required global objects and contains initial Arduino setup()
and Arduino loop() functions, as well as interrput code for OC0B and OC1B.
Also includes declarations of optional array of Turn-Outs and optional array of Sensors

SerialCommand:    contains methods to read and interpret text commands from the serial line,
process those instructions, and, if necessary call appropriate Packet RegisterList methods
to update either the Main Track or Programming Track Packet Registers

PacketRegister:   contains methods to load, store, and update Packet Registers with DCC instructions

CurrentMonitor:   contains methods to separately monitor and report the current drawn from CHANNEL A and
CHANNEL B of the Arduino Motor Shield's, and shut down power if a short-circuit overload
is detected

Accessories:      contains methods to operate and store the status of any optionally-defined turnouts controlled
by a DCC stationary accessory decoder.

Sensor:           contains methods to monitor and report on the status of optionally-defined infrared
sensors embedded in the Main Track and connected to various pins on the Arudino Uno

Outputs:          contains methods to configure one or more Arduino pins as an output for your own custom use

EEStore:          contains methods to store, update, and load various DCC settings and status
(e.g. the states of all defined turnouts) in the EEPROM for recall after power-up

DCC++ BASE STATION is configured through the Config.h file that contains all user-definable parameters

**********************************************************************/

#include "DcDccNanoController.h"
#include "Arduino.h"
#include "ControllerDccpp.hpp"

// SET UP COMMUNICATIONS INTERFACE - FOR STANDARD SERIAL, NOTHING NEEDS TO BE DONE

#if COMM_TYPE == 1
byte mac[] = MAC_ADDRESS;                                // Create MAC address (to be used for DHCP when initializing server)
EthernetServer INTERFACE(ETHERNET_PORT);                  // Create and instance of an EnternetServer
#endif

														  // NEXT DECLARE GLOBAL OBJECTS TO PROCESS AND STORE DCC PACKETS AND MONITOR TRACK CURRENTS.
														  // NOTE REGISTER LISTS MUST BE DECLARED WITH "VOLATILE" QUALIFIER TO ENSURE THEY ARE PROPERLY UPDATED BY INTERRUPT ROUTINES

volatile RegisterList ControllerDccpp::mainRegs(MAX_MAIN_REGISTERS);    // create list of registers for MAX_MAIN_REGISTER Main Track Packets
volatile RegisterList ControllerDccpp::progRegs(2);                     // create a shorter list of only two registers for Program Track Packets

CurrentMonitor mainMonitor;  // create monitor for current on Main Track
CurrentMonitor progMonitor;  // create monitor for current on Program Track

ControllerDccpp::ControllerDccpp() 
{ 
	this->programMode = false; 
	this->maxSpeed = 127; 

	DccppConfig::SignalEnablePinMain = 255;
	DccppConfig::CurrentMonitorMain = 255;

	DccppConfig::SignalEnablePinProg = 255;
	DccppConfig::CurrentMonitorProg = 255;

	DccppConfig::DirectionMotorA = 255;
	DccppConfig::DirectionMotorB = 255;

	mainMonitor.begin(255, "");
	progMonitor.begin(255, "");
}
	
static bool first = true;

///////////////////////////////////////////////////////////////////////////////
// MAIN ARDUINO LOOP
///////////////////////////////////////////////////////////////////////////////

void ControllerDccpp::loop()
{
	//SerialCommand::process();              // check for, and process, and new serial commands

	if (first)
	{
		first = false;
#ifdef DDC_DEBUG_MODE
		showConfiguration();
#endif
	}

	if (CurrentMonitor::checkTime())
	{      // if sufficient time has elapsed since last update, check current draw on Main and Program Tracks 
		mainMonitor.check();
		progMonitor.check();
	}

//	Sensor::check();    // check sensors for activate/de-activate

}

///////////////////////////////////////////////////////////////////////////////
// INITIAL SETUP
///////////////////////////////////////////////////////////////////////////////

// For Arduino or Polulu shields, signalPinMain must be connected to Direction motor A, and signalPinProg to Direction motor B
// If a track is not connected,  main or prog, the signalPin should stay to default at 255.
// For H bridge connected directly to the pins, like LMD18200, signalPin and Direction motor should have the same pin number.

// For Arduino Motor Shield
// beginMain(MOTOR_SHIELD_DIRECTION_MOTOR_CHANNEL_PIN_A, MOTOR_SHIELD_SIGNAL_ENABLE_PIN_MAIN, MOTOR_SHIELD_CURRENT_MONITOR_PIN_MAIN);
// beginProg(MOTOR_SHIELD_DIRECTION_MOTOR_CHANNEL_PIN_B, DCC_SIGNAL_PIN_PROG, MOTOR_SHIELD_SIGNAL_ENABLE_PIN_PROG, MOTOR_SHIELD_CURRENT_MONITOR_PIN_PROG);

// For Polulu Motor Shield
// beginMain(POLULU_DIRECTION_MOTOR_CHANNEL_PIN_A, DCC_SIGNAL_PIN_MAIN, POLULU_SIGNAL_ENABLE_PIN_MAIN, POLULU_CURRENT_MONITOR_PIN_MAIN);
// beginProg(POLULU_DIRECTION_MOTOR_CHANNEL_PIN_B, DCC_SIGNAL_PIN_PROG, POLULU_SIGNAL_ENABLE_PIN_PROG, POLULU_CURRENT_MONITOR_PIN_PROG);

// For single LMD18200
// beginMain(255, DCC_SIGNAL_PIN_MAIN, 3, A0);

// For double LMD18200
// beginMain(255, DCC_SIGNAL_PIN_MAIN, 3, A0);
// beginProg(255, DCC_SIGNAL_PIN_PROG, 11, A1);

void ControllerDccpp::beginMain(uint8_t inDirectionMotor, uint8_t Dummy, uint8_t inSignalEnable, uint8_t inCurrentMonitor)
{
	DccppConfig::DirectionMotorA = inDirectionMotor;
	DccppConfig::SignalEnablePinMain = inSignalEnable;	// PWM
	DccppConfig::CurrentMonitorMain = inCurrentMonitor;

	// If no main line, exit.
	if (DccppConfig::SignalEnablePinMain == 255)
		return;

	mainMonitor.begin(DccppConfig::CurrentMonitorMain, (char *) "<p2>");

	// CONFIGURE TIMER_1 TO OUTPUT 50% DUTY CYCLE DCC SIGNALS ON OC1B INTERRUPT PINS

	// Direction Pin for Motor Shield Channel A - MAIN OPERATIONS TRACK
	// Controlled by Arduino 16-bit TIMER 1 / OC1B Interrupt Pin
	// Values for 16-bit OCR1A and OCR1B registers calibrated for 1:1 prescale at 16 MHz clock frequency
	// Resulting waveforms are 200 microseconds for a ZERO bit and 116 microseconds for a ONE bit with exactly 50% duty cycle

#define DCC_ZERO_BIT_TOTAL_DURATION_TIMER1 3199
#define DCC_ZERO_BIT_PULSE_DURATION_TIMER1 1599

#define DCC_ONE_BIT_TOTAL_DURATION_TIMER1 1855
#define DCC_ONE_BIT_PULSE_DURATION_TIMER1 927
	if (DccppConfig::DirectionMotorA != 255)
	{
		pinMode(DccppConfig::DirectionMotorA, INPUT);      // ensure this pin is not active! Direction will be controlled by DCC SIGNAL instead (below)
		digitalWrite(DccppConfig::DirectionMotorA, LOW);
	}

	pinMode(DCC_SIGNAL_PIN_MAIN, OUTPUT);      // THIS ARDUINO OUPUT PIN MUST BE PHYSICALLY CONNECTED TO THE PIN FOR DIRECTION-A OF MOTOR CHANNEL-A

#ifndef VISUALSTUDIO
	bitSet(TCCR1A, WGM10);     // set Timer 1 to FAST PWM, with TOP=OCR1A
	bitSet(TCCR1A, WGM11);
	bitSet(TCCR1B, WGM12);
	bitSet(TCCR1B, WGM13);

	bitSet(TCCR1A, COM1B1);    // set Timer 1, OC1B (pin 10/UNO, pin 12/MEGA) to inverting toggle (actual direction is arbitrary)
	bitSet(TCCR1A, COM1B0);

	bitClear(TCCR1B, CS12);    // set Timer 1 prescale=1
	bitClear(TCCR1B, CS11);
	bitSet(TCCR1B, CS10);

	OCR1A = DCC_ONE_BIT_TOTAL_DURATION_TIMER1;
	OCR1B = DCC_ONE_BIT_PULSE_DURATION_TIMER1;
#endif // !VISUALSTUDIO

	pinMode(DccppConfig::SignalEnablePinMain, OUTPUT);   // master enable for motor channel A

	mainRegs.loadPacket(this->speedRegister, RegisterList::idlePacket, 2, 0);    // load idle packet into register 1
	mainRegs.nextReg = NULL;
	mainRegs.loadPacket(this->functionRegister, RegisterList::idlePacket, 2, 0);    // load idle packet into register 2
	mainRegs.nextReg = NULL;

#ifndef VISUALSTUDIO
	bitSet(TIMSK1, OCIE1B);    // enable interrupt vector for Timer 1 Output Compare B Match (OCR1B)    
#endif // !VISUALSTUDIO

	digitalWrite(DccppConfig::SignalEnablePinMain, LOW);
}

void ControllerDccpp::beginProg(uint8_t inDirectionMotor, uint8_t inSignalPin, uint8_t inSignalEnable, uint8_t inCurrentMonitor)
{
	DccppConfig::DirectionMotorB = inDirectionMotor;
	DccppConfig::SignalEnablePinProg = inSignalEnable;
	DccppConfig::CurrentMonitorProg = inCurrentMonitor;

	// If no prog line, exit.
	if (DccppConfig::SignalEnablePinProg == 255)
		return;

	progMonitor.begin(DccppConfig::CurrentMonitorProg, (char *) "<p3>");

	// CONFIGURE EITHER TIMER_0 (UNO) OR TIMER_3 (MEGA) TO OUTPUT 50% DUTY CYCLE DCC SIGNALS ON OC0B (UNO) OR OC3B (MEGA) INTERRUPT PINS

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)      // Configuration for UNO

	// Directon Pin for Motor Shield Channel B - PROGRAMMING TRACK
	// Controlled by Arduino 8-bit TIMER 0 / OC0B Interrupt Pin
	// Values for 8-bit OCR0A and OCR0B registers calibrated for 1:64 prescale at 16 MHz clock frequency
	// Resulting waveforms are 200 microseconds for a ZERO bit and 116 microseconds for a ONE bit with as-close-as-possible to 50% duty cycle

#define DCC_ZERO_BIT_TOTAL_DURATION_TIMER0 49
#define DCC_ZERO_BIT_PULSE_DURATION_TIMER0 24

#define DCC_ONE_BIT_TOTAL_DURATION_TIMER0 28
#define DCC_ONE_BIT_PULSE_DURATION_TIMER0 14

	if (DccppConfig::DirectionMotorB != 255)
	{
		pinMode(DccppConfig::DirectionMotorB, INPUT);      // ensure this pin is not active! Direction will be controlled by DCC SIGNAL instead (below)
		digitalWrite(DccppConfig::DirectionMotorB, LOW);
	}

	pinMode(DCC_SIGNAL_PIN_PROG, OUTPUT);      // THIS ARDUINO OUTPUT PIN MUST BE PHYSICALLY CONNECTED TO THE PIN FOR DIRECTION-B OF MOTOR CHANNEL-B

#ifndef VISUALSTUDIO
	bitSet(TCCR0A, WGM00);     // set Timer 0 to FAST PWM, with TOP=OCR0A
	bitSet(TCCR0A, WGM01);
	bitSet(TCCR0B, WGM02);

	bitSet(TCCR0A, COM0B1);    // set Timer 0, OC0B (pin 5) to inverting toggle (actual direction is arbitrary)
	bitSet(TCCR0A, COM0B0);

	bitClear(TCCR0B, CS02);    // set Timer 0 prescale=64
	bitSet(TCCR0B, CS01);
	bitSet(TCCR0B, CS00);

	OCR0A = DCC_ONE_BIT_TOTAL_DURATION_TIMER0;
	OCR0B = DCC_ONE_BIT_PULSE_DURATION_TIMER0;
#endif // !VISUALSTUDIO

	pinMode(DccppConfig::SignalEnablePinProg, OUTPUT);   // master enable for motor channel B

	progRegs.loadPacket(1, RegisterList::idlePacket, 2, 0);    // load idle packet into register 1    

#ifndef VISUALSTUDIO
	bitSet(TIMSK0, OCIE0B);    // enable interrupt vector for Timer 0 Output Compare B Match (OCR0B)
#endif // !VISUALSTUDIO

#else      // Configuration for MEGA

	// Directon Pin for Motor Shield Channel B - PROGRAMMING TRACK
	// Controlled by Arduino 16-bit TIMER 3 / OC3B Interrupt Pin
	// Values for 16-bit OCR3A and OCR3B registers calibrated for 1:1 prescale at 16 MHz clock frequency
	// Resulting waveforms are 200 microseconds for a ZERO bit and 116 microseconds for a ONE bit with exactly 50% duty cycle

#define DCC_ZERO_BIT_TOTAL_DURATION_TIMER3 3199
#define DCC_ZERO_BIT_PULSE_DURATION_TIMER3 1599

#define DCC_ONE_BIT_TOTAL_DURATION_TIMER3 1855
#define DCC_ONE_BIT_PULSE_DURATION_TIMER3 927

	pinMode(DccppConfig::DirectionMotorB, INPUT);      // ensure this pin is not active! Direction will be controlled by DCC SIGNAL instead (below)
	digitalWrite(DccppConfig::DirectionMotorB, LOW);

	pinMode(DCC_SIGNAL_PIN_PROG, OUTPUT);      // THIS ARDUINO OUTPUT PIN MUST BE PHYSICALLY CONNECTED TO THE PIN FOR DIRECTION-B OF MOTOR CHANNEL-B

#ifndef VISUALSTUDIO
	bitSet(TCCR3A, WGM30);     // set Timer 3 to FAST PWM, with TOP=OCR3A
	bitSet(TCCR3A, WGM31);
	bitSet(TCCR3B, WGM32);
	bitSet(TCCR3B, WGM33);

	bitSet(TCCR3A, COM3B1);    // set Timer 3, OC3B (pin 2) to inverting toggle (actual direction is arbitrary)
	bitSet(TCCR3A, COM3B0);

	bitClear(TCCR3B, CS32);    // set Timer 3 prescale=1
	bitClear(TCCR3B, CS31);
	bitSet(TCCR3B, CS30);

	OCR3A = DCC_ONE_BIT_TOTAL_DURATION_TIMER3;
	OCR3B = DCC_ONE_BIT_PULSE_DURATION_TIMER3;
#endif // !VISUALSTUDIO

	pinMode(DccppConfig::SignalEnablePinProg, OUTPUT);   // master enable for motor channel B

	progRegs.loadPacket(1, RegisterList::idlePacket, 2, 0);    // load idle packet into register 1    

#ifndef VISUALSTUDIO
	bitSet(TIMSK3, OCIE3B);    // enable interrupt vector for Timer 3 Output Compare B Match (OCR3B)    
#endif // !VISUALSTUDIO

#endif
	digitalWrite(DccppConfig::SignalEnablePinProg, LOW);
}

void ControllerDccpp::begin()
{
#ifdef SDCARD_CS
	pinMode(SDCARD_CS, OUTPUT);
	digitalWrite(SDCARD_CS, HIGH);     // Deselect the SD card
#endif

	//EEStore::init();                                          // initialize and load Turnout and Sensor definitions stored in EEPROM
	this->speedRegister = 1;
	this->functionRegister = 2;

#if COMM_TYPE == 1
#ifdef IP_ADDRESS
	Ethernet.begin(mac, IP_ADDRESS);           // Start networking using STATIC IP Address
#else
	Ethernet.begin(mac);                      // Start networking using DHCP to get an IP Address
#endif
	INTERFACE.begin();
#endif

#ifdef DDC_DEBUG_MODE
	Serial.println(F("Dcc++ mode."));
	pinMode(LED_BUILTIN, OUTPUT);
#endif

} // begin

  ///////////////////////////////////////////////////////////////////////////////
  // DEFINE THE INTERRUPT LOGIC THAT GENERATES THE DCC SIGNAL
  ///////////////////////////////////////////////////////////////////////////////

  // The code below will be called every time an interrupt is triggered on OCNB, where N can be 0 or 1. 
  // It is designed to read the current bit of the current register packet and
  // updates the OCNA and OCNB counters of Timer-N to values that will either produce
  // a long (200 microsecond) pulse, or a short (116 microsecond) pulse, which respectively represent
  // DCC ZERO and DCC ONE bits.

  // These are hardware-driven interrupts that will be called automatically when triggered regardless of what
  // DCC++ BASE STATION was otherwise processing.  But once inside the interrupt, all other interrupt routines are temporarily diabled.
  // Since a short pulse only lasts for 116 microseconds, and there are TWO separate interrupts
  // (one for Main Track Registers and one for the Program Track Registers), the interrupt code must complete
  // in much less than 58 microsends, otherwise there would be no time for the rest of the program to run.  Worse, if the logic
  // of the interrupt code ever caused it to run longer than 58 microsends, an interrupt trigger would be missed, the OCNA and OCNB
  // registers would not be updated, and the net effect would be a DCC signal that keeps sending the same DCC bit repeatedly until the
  // interrupt code completes and can be called again.

  // A significant portion of this entire program is designed to do as much of the heavy processing of creating a properly-formed
  // DCC bit stream upfront, so that the interrupt code below can be as simple and efficient as possible.

  // Note that we need to create two very similar copies of the code --- one for the Main Track OC1B interrupt and one for the
  // Programming Track OCOB interrupt.  But rather than create a generic function that incurrs additional overhead, we create a macro
  // that can be invoked with proper paramters for each interrupt.  This slightly increases the size of the code base by duplicating
  // some of the logic for each interrupt, but saves additional time.

  // As structured, the interrupt code below completes at an average of just under 6 microseconds with a worse-case of just under 11 microseconds
  // when a new register is loaded and the logic needs to switch active register packet pointers.

  // THE INTERRUPT CODE MACRO:  R=REGISTER LIST (mainRegs or progRegs), and N=TIMER (0 or 1)

#define DCC_SIGNAL(R,N) \
  if(R.currentBit==R.currentReg->activePacket->nBits){    /* IF no more bits in this DCC Packet */ \
    R.currentBit=0;                                       /*   reset current bit pointer and determine which Register and Packet to process next--- */ \
	if (R.nRepeat>0 && R.currentReg == R.reg) {               /*   IF current Register is first Register AND should be repeated */ \
		R.nRepeat--;                                        /*     decrement repeat count; result is this same Packet will be repeated */ \
	} \
	else if (R.nextReg != NULL) {                           /*   ELSE IF another Register has been updated */ \
		R.currentReg = R.nextReg;                             /*     update currentReg to nextReg */ \
		R.nextReg = NULL;                                     /*     reset nextReg to NULL */ \
		R.tempPacket = R.currentReg->activePacket;            /*     flip active and update Packets */ \
		R.currentReg->activePacket = R.currentReg->updatePacket; \
		R.currentReg->updatePacket = R.tempPacket; \
	} \
	else {                                               /*   ELSE simply move to next Register */ \
		if (R.currentReg == R.maxLoadedReg)                    /*     BUT IF this is last Register loaded */ \
			R.currentReg = R.reg;                               /*       first reset currentReg to base Register, THEN */ \
			R.currentReg++;                                     /*     increment current Register (note this logic causes Register[0] to be skipped when simply cycling through all Registers) */ \
		}                                                     /*   END-ELSE */ \
	}                                                       /* END-IF: currentReg, activePacket, and currentBit should now be properly set to point to next DCC bit */ \
	\
	if (R.currentReg->activePacket->buf[R.currentBit / 8] & R.bitMask[R.currentBit % 8]) {     /* IF bit is a ONE */ \
		OCR ## N ## A = DCC_ONE_BIT_TOTAL_DURATION_TIMER ## N;                               /*   set OCRA for timer N to full cycle duration of DCC ONE bit */ \
    OCR ## N ## B=DCC_ONE_BIT_PULSE_DURATION_TIMER ## N;                               /*   set OCRB for timer N to half cycle duration of DCC ONE but */ \
  } else{                                                                              /* ELSE it is a ZERO */ \
    OCR ## N ## A=DCC_ZERO_BIT_TOTAL_DURATION_TIMER ## N;                              /*   set OCRA for timer N to full cycle duration of DCC ZERO bit */ \
    OCR ## N ## B=DCC_ZERO_BIT_PULSE_DURATION_TIMER ## N;                              /*   set OCRB for timer N to half cycle duration of DCC ZERO bit */ \
  }                                                                                    /* END-ELSE */ \
	\
	R.currentBit++;     /* point to next bit in current Packet */

///////////////////////////////////////////////////////////////////////////////
// NOW USE THE ABOVE MACRO TO CREATE THE CODE FOR EACH INTERRUPT

ISR(TIMER1_COMPB_vect) {              // set interrupt service for OCR1B of TIMER-1 which flips direction bit of Motor Shield Channel A controlling Main Track
	DCC_SIGNAL(ControllerDccpp::mainRegs, 1)
}

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)      // Configuration for UNO

ISR(TIMER0_COMPB_vect) {              // set interrupt service for OCR1B of TIMER-0 which flips direction bit of Motor Shield Channel B controlling Prog Track
	DCC_SIGNAL(ControllerDccpp::progRegs, 0)
}

#else      // Configuration for MEGA

ISR(TIMER3_COMPB_vect) {              // set interrupt service for OCR3B of TIMER-3 which flips direction bit of Motor Shield Channel B controlling Prog Track
	DCC_SIGNAL(ControllerDccpp::progRegs, 3)
}

#endif

#ifdef DDC_DEBUG_MODE
///////////////////////////////////////////////////////////////////////////////
// PRINT CONFIGURATION INFO TO SERIAL PORT REGARDLESS OF INTERFACE TYPE
// - ACTIVATED ON STARTUP IF SHOW_CONFIG_PIN IS TIED HIGH 

void ControllerDccpp::showConfiguration() 
{

	int mac_address[] = MAC_ADDRESS;

	Serial.print(F("\n*** DCC++ CONFIGURATION ***\n"));

	Serial.print(F("\nVERSION:      "));
	Serial.print(VERSION);
	Serial.print(F("\nCOMPILED:     "));
	Serial.print(__DATE__);
	Serial.print(F(" "));
	Serial.print(__TIME__);

	//Serial.print(F("\nARDUINO:      "));
	//Serial.print(ARDUINO_TYPE);

	//Serial.print(F("\n\nMOTOR SHIELD: "));
	//Serial.print(MOTOR_SHIELD_NAME);

	if (DccppConfig::SignalEnablePinMain!= 255)
	{
		Serial.print(F("\n\nDCC SIG MAIN(DIR): "));
		Serial.print(DCC_SIGNAL_PIN_MAIN);
		Serial.print(F("\n   DIRECTION: "));
		Serial.print(DccppConfig::DirectionMotorA);
		Serial.print(F("\n      ENABLE(PWM): "));
		Serial.print(DccppConfig::SignalEnablePinMain);
		Serial.print(F("\n     CURRENT: "));
		Serial.print(DccppConfig::CurrentMonitorMain);
	}

	if (DccppConfig::SignalEnablePinProg!= 255)
	{
		Serial.print(F("\n\nDCC SIG PROG(DIR): "));
		Serial.print(DCC_SIGNAL_PIN_PROG);
		Serial.print(F("\n   DIRECTION: "));
		Serial.print(DccppConfig::DirectionMotorB);
		Serial.print(F("\n      ENABLE(PWM): "));
		Serial.print(DccppConfig::SignalEnablePinProg);
		Serial.print(F("\n     CURRENT: "));
		Serial.print(DccppConfig::CurrentMonitorProg);
	}
#if 0
	Serial.print(F("\n\nNUM TURNOUTS: "));
	Serial.print(EEStore::eeStore->data.nTurnouts);
	Serial.print(F("\n     SENSORS: "));
	Serial.print(EEStore::eeStore->data.nSensors);
	Serial.print(F("\n     OUTPUTS: "));
	Serial.print(EEStore::eeStore->data.nOutputs);
#endif
	/*
	Serial.print(F("\n\nINTERFACE:    "));
#if COMM_TYPE == 0
	Serial.print(F("SERIAL"));
#elif COMM_TYPE == 1
	Serial.print(COMM_SHIELD_NAME);
	Serial.print(F("\nMAC ADDRESS:  "));
	for (int i = 0; i<5; i++) {
		Serial.print(mac_address[i], HEX);
		Serial.print(F(":"));
	}
	Serial.print(mac_address[5], HEX);
	Serial.print(F("\nPORT:         "));
	Serial.print(ETHERNET_PORT);
	Serial.print(F("\nIP ADDRESS:   "));

#ifdef IP_ADDRESS
	Ethernet.begin(mac, IP_ADDRESS);           // Start networking using STATIC IP Address
#else
	Ethernet.begin(mac);                      // Start networking using DHCP to get an IP Address
#endif     

	Serial.print(Ethernet.localIP());

#ifdef IP_ADDRESS
	Serial.print(F(" (STATIC)"));
#else
	Serial.print(F(" (DHCP)"));
#endif

#endif
	Serial.print(F("\n\nPROGRAM HALTED - PLEASE RESTART ARDUINO"));

	while (true);
	*/

	Serial.println("");
}
#endif

void ControllerDccpp::SetSpeedRaw()
{
	byte state = HIGH;

	if (this->panicStopped)
	{
		this->mainRegs.setThrottle(this->speedRegister, this->pControlled->GetDccId(), 1, this->pControlled->GetDirectionToLeft());
		state = LOW;
	}
	else
		this->mainRegs.setThrottle(this->speedRegister, this->pControlled->GetDccId(), this->pControlled->GetMappedSpeed(), this->pControlled->GetDirectionToLeft());

	if (DccppConfig::SignalEnablePinMain != 255)
		digitalWrite(DccppConfig::SignalEnablePinMain, state);
	if (DccppConfig::SignalEnablePinProg != 255)
		digitalWrite(DccppConfig::SignalEnablePinProg, state);
}

bool ControllerDccpp::SetSpeed(int inNewSpeed)
{
	int val = 0;
	
	if (inNewSpeed > 0)
		val = map(inNewSpeed, 0, this->pControlled->GetSteps(), 2, 127);

	if (this->pControlled->GetMappedSpeed() == val)
		return false;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDccpp SetSpeed "));
	Serial.print(inNewSpeed);
	Serial.print(F("/"));
	Serial.print(this->pControlled->GetSteps());
	Serial.print(F(" (in Dcc "));
	Serial.print(val);
	Serial.println(F(" )"));
#endif

	this->pControlled->SetMappedSpeed(val);
	this->SetSpeedRaw();

	return true;
}

bool ControllerDccpp::SetDirection(bool inToLeft)
{
	if (this->pControlled->GetDirectionToLeft() == inToLeft)
		return false;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDccpp SetDirection "));
	Serial.println(inToLeft ? F("Forward") : F("Reverse"));
#endif

	this->pControlled->SetDirectionToLeft(inToLeft);
	this->SetSpeed(0);
	return true;
}

void ControllerDccpp::SetFunctionsRaw()
{
	byte flags = 0;

	byte oneByte1 = 128;	// Group one functions F0-F4
	byte twoByte1 = 176;	// Group two F5-F8
	byte threeByte1 = 160;	// Group three F9-F12
	byte fourByte2 = 0;		// Group four F13-F20
	byte fiveByte2 = 0;		// Group five F21-F28

	for (int i = 0; i < FUNCTION_NUMBER; i++)
	{
		byte func = this->pControlled->Functions[i].DccIdFunction;
		if (func <= 4)
		{
			/*
			*	To set functions F0 - F4 on(= 1) or off(= 0) :
			*
			*    BYTE1 : 128 + F1 * 1 + F2 * 2 + F3 * 4 + F4 * 8 + F0 * 16
			* BYTE2 : omitted
			*/

			flags |= 1;
			if (this->pControlled->Functions[i].IsActivated())
			{
				if (func == 0)
					oneByte1 += 16;
				else
					oneByte1 += (1 << (func - 1));
			}
		}
		else if (func <= 8)
		{
			/*
			*	To set functions F5 - F8 on(= 1) or off(= 0) :
			*
			*    BYTE1 : 176 + F5 * 1 + F6 * 2 + F7 * 4 + F8 * 8
			* BYTE2 : omitted
			*/

			flags |= 2;
			if (this->pControlled->Functions[i].IsActivated())
				twoByte1 += (1 << (func - 5));
		}
		else if (func <= 12)
		{
			/*
			*    To set functions F9 - F12 on(= 1) or off(= 0) :
			*
			*    BYTE1 : 160 + F9 * 1 + F10 * 2 + F11 * 4 + F12 * 8
			* BYTE2 : omitted
			*/

			flags |= 4;
			if (this->pControlled->Functions[i].IsActivated())
				threeByte1 += (1 << (func - 9));
		}
		else if (func <= 20)
		{
			/*
			*    To set functions F13 - F20 on(= 1) or off(= 0) :
			*
			*    BYTE1 : 222
			* BYTE2 : F13 * 1 + F14 * 2 + F15 * 4 + F16 * 8 + F17 * 16 + F18 * 32 + F19 * 64 + F20 * 128
			*/

			flags |= 8;
			if (this->pControlled->Functions[i].IsActivated())
				fourByte2 += (1 << (func - 13));
		}
		else if (func <= 28)
		{
			/*
			*    To set functions F21 - F28 on(= 1) of off(= 0) :
			*
			*    BYTE1 : 223
			* BYTE2 : F21 * 1 + F22 * 2 + F23 * 4 + F24 * 8 + F25 * 16 + F26 * 32 + F27 * 64 + F28 * 128
			*/

			flags |= 16;
			if (this->pControlled->Functions[i].IsActivated())
				fiveByte2 += (1 << (func - 21));
		}
	}

	if (flags & 1)
		this->mainRegs.setFunction(this->functionRegister, this->pControlled->GetDccId(), oneByte1, -1);
	if (flags & 2)
		this->mainRegs.setFunction(this->functionRegister, this->pControlled->GetDccId(), twoByte1, -1);
	if (flags & 4)
		this->mainRegs.setFunction(this->functionRegister, this->pControlled->GetDccId(), threeByte1, -1);
	if (flags & 8)
		this->mainRegs.setFunction(this->functionRegister, this->pControlled->GetDccId(), 222, fourByte2);
	if (flags & 16)
		this->mainRegs.setFunction(this->functionRegister, this->pControlled->GetDccId(), 223, fiveByte2);
}


// If inFunctionNumber is 255, just refresh the current state of all the functions.
void ControllerDccpp::SetFunction(byte inFunctionNumber, bool inActivate)
{
	if (inFunctionNumber != 255) // if a function have to change its state...
	{
		Function &f = this->pControlled->Functions[inFunctionNumber];
		f.SetActivated(inActivate);

#ifdef DDC_DEBUG_MODE
		Serial.print(F("ControllerDccpp SetFunction "));
		Serial.print(this->pControlled->Functions[inFunctionNumber].DccIdFunction);
		Serial.println(this->pControlled->Functions[inFunctionNumber].IsActivated() ? F(" On") : F(" Off"));
#endif
	}

	this->SetFunctionsRaw();
}

void ControllerDccpp::SetAccessory(int inAddress, byte inSubAddress, bool inActivate)
{
	this->mainRegs.setAccessory(inAddress, inSubAddress, inActivate?1:0);
}

void ControllerDccpp::PanicStop(bool inStop)
{
	this->panicStopped = inStop;

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDccpp PanicStop "));
	Serial.println(inStop ? F("pressed"):F("canceled"));
#endif

	this->SetSpeedRaw();
	if (!inStop)
		this->SetFunctionsRaw();
}

void ControllerDccpp::StartProgramMode()
{
	this->programMode = true;
}

void ControllerDccpp::EndProgramMode()
{
	this->programMode = false;
}

void ControllerDccpp::WriteCv(int inCv, byte inValue)
{
	this->mainRegs.writeCVByte(inCv, inValue, 100, 101);

#ifdef DDC_DEBUG_MODE
	Serial.print(F("ControllerDccpp WriteCv "));
	Serial.print(inCv);
	Serial.print(F(" : "));
	Serial.println(inValue);
#endif
}

int ControllerDccpp::ReadCv(byte inCv)
{
	return this->mainRegs.readCVmain(inCv, 100+inCv, 100+inCv);
}

