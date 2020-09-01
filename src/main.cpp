#include <Arduino.h>

const byte MCU_EMULATION_PIN = 2;
const byte REMOTE_EMULATION_PIN = 3;
const byte TEST_HIGH_PIN = 4;
const byte BYTES_IN_PACKET = 6;

// mock data packet format: pin#, 6 data bytes, delay in ms
// REU-16 FUA-IS
const byte MOCK_DATA_SIMPLE[][1 + BYTES_IN_PACKET + 1] = {
	{REMOTE_EMULATION_PIN, 0xc7, 0x01, 0x85, 0xe0, 0x20, 0x83, 10}, // master, off
	//{REMOTE_EMULATION_PIN, 0x97, 0xc1, 0x85, 0xe0, 0x20, 0x13, 10}, // master, on
	//{REMOTE_EMULATION_PIN, 0x97, 0xc1, 0x1c, 0xe0, 0x20, 0x8a, 10}, // master, in use
	{MCU_EMULATION_PIN,    0x86, 0x80, 0x80, 0xdf, 0xbf, 0xe6, 50}, // mcu
	{REMOTE_EMULATION_PIN, 0x02, 0x80, 0x80, 0x7f, 0xbf, 0xc2, 50}}; // second control panel
// genmeim data set
const byte MOCK_DATA_GENMEIM[][1 + BYTES_IN_PACKET + 1] = {
	{REMOTE_EMULATION_PIN, 0xab, 0x80, 0x80, 0x80, 0x23, 0x08, 10}, // master, extra info 1
	{REMOTE_EMULATION_PIN, 0x67, 0x40, 0x0e, 0x26, 0x20, 0x2f, 10}, // master, main
	{MCU_EMULATION_PIN,    0x02, 0x80, 0x80, 0xbf, 0xbf, 0x02, 140}, // mcu
	{REMOTE_EMULATION_PIN, 0xcb, 0x40, 0x80, 0x80, 0x20, 0xab, 10}, // master, extra info 2
	{REMOTE_EMULATION_PIN, 0x67, 0x40, 0x0e, 0x26, 0x20, 0x2f, 10}, // master, main
	{MCU_EMULATION_PIN,    0x02, 0x80, 0x80, 0xbf, 0xbf, 0x02, 10}, // mcu
	{REMOTE_EMULATION_PIN, 0x02, 0x80, 0x80, 0x7f, 0xbf, 0xc2, 100}}; // second control panel


#define MOCK_DATA MOCK_DATA_SIMPLE

const byte DATA_COUNT = sizeof(MOCK_DATA) / sizeof(MOCK_DATA[0]);

const int INIT_PULSE = 850;
const int SHORT_PULSE = 150;
const int LONG_PULSE = 450;

void setup()
{
	digitalWrite(MCU_EMULATION_PIN, LOW);
	pinMode(MCU_EMULATION_PIN, OUTPUT); // the MCU pin in the real unit bahaves like an open drain

	digitalWrite(REMOTE_EMULATION_PIN, LOW);
	pinMode(REMOTE_EMULATION_PIN, OUTPUT);

	// just a test pin that always high
	digitalWrite(TEST_HIGH_PIN, HIGH);
	pinMode(TEST_HIGH_PIN, OUTPUT);
}

void sendPulse(const byte pin, unsigned int len)
{
	if (pin == MCU_EMULATION_PIN) // the MCU pin in the real unit bahaves like an open drain
	{
		pinMode(MCU_EMULATION_PIN, INPUT);
		delayMicroseconds(len);
		pinMode(MCU_EMULATION_PIN, OUTPUT);
	}
	else
	{
		digitalWrite(pin, HIGH);
		delayMicroseconds(len);
		digitalWrite(pin, LOW);
	}	
}

void writePacket(const byte pin, const byte * data, const byte len) {
	// send init
	sendPulse(pin, INIT_PULSE);
	// send bytes
	for(byte i = 0; i < len; i++) {
		// send byte
		for(byte bit = 0; bit < 8; bit++) {
			// send bit
			const byte value = data[i] & (1 << bit);
			if (value) {
				delayMicroseconds(SHORT_PULSE);
				sendPulse(pin, LONG_PULSE);
			} else {
				delayMicroseconds(LONG_PULSE);
				sendPulse(pin, SHORT_PULSE);
			}
		}
	}
}

void loop()
{
	for(byte i = 0; i < DATA_COUNT; i++) {
		writePacket(MOCK_DATA[i][0], &MOCK_DATA[i][1], BYTES_IN_PACKET);
		delay(MOCK_DATA[i][1 + BYTES_IN_PACKET]);
	}
}
