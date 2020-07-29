#include <Arduino.h>

const byte MCU_EMULATION_PIN = 2;
const byte REMOTE_EMULATION_PIN = 3;
const byte TEST_HIGH_PIN = 4;
const byte BYTES_IN_PACKET = 6;

const byte MOCK_DATA[][1 + BYTES_IN_PACKET + 1] = { // pin, 6 data bytes, delay in ms
	{REMOTE_EMULATION_PIN, 0xc7, 0x01, 0x85, 0xe0, 0x20, 0x83, 10}, // master
	{MCU_EMULATION_PIN,    0x86, 0x80, 0x80, 0xdf, 0xbf, 0xe6, 50}, // mcu
	{REMOTE_EMULATION_PIN, 0x02, 0x80, 0x80, 0x7f, 0xbf, 0xc2, 50}}; // second control panel

const byte DATA_COUNT = sizeof(MOCK_DATA) / sizeof(MOCK_DATA[0]);

const int INIT_PULSE = 850;
const int SHORT_PULSE = 150;
const int LONG_PULSE = 450;

void setup()
{
	digitalWrite(MCU_EMULATION_PIN, LOW);
	pinMode(MCU_EMULATION_PIN, OUTPUT);
	digitalWrite(REMOTE_EMULATION_PIN, LOW);
	pinMode(REMOTE_EMULATION_PIN, OUTPUT);
	digitalWrite(TEST_HIGH_PIN, HIGH);
	pinMode(TEST_HIGH_PIN, OUTPUT);
}

void writePacket(const byte pin, const byte * data, const byte len) {
	// send init
	digitalWrite(pin, HIGH);
	delayMicroseconds(INIT_PULSE);
	digitalWrite(pin, LOW);
	// send bytes
	for(byte i = 0; i < len; i++) {
		// send byte
		for(byte bit = 0; bit < 8; bit++) {
			// send bit
			const byte value = data[i] & (1 << bit);
			if (value) {
				delayMicroseconds(SHORT_PULSE);
				digitalWrite(pin, HIGH);
				delayMicroseconds(LONG_PULSE);
				digitalWrite(pin, LOW);
			} else {
				delayMicroseconds(LONG_PULSE);
				digitalWrite(pin, HIGH);
				delayMicroseconds(SHORT_PULSE);
				digitalWrite(pin, LOW);
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
