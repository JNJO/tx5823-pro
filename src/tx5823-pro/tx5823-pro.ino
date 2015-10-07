/*

tx5823-pro

Description: The tx5823-pro transmitter module is configured via IR by the
parent project x5808-pro-diversity.

Transmitter Project:
https://github.com/sheaivey/tx5823-pro

Parent Receiver Project:
https://github.com/sheaivey/rx5808-pro-diversity

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <avr/pgmspace.h>
#include <EEPROM.h>

#include "settings.h"

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

#include "screens.h"
screens drawScreen;

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
    0x7981,    0x758D,    0x7199,    0x6DA5,    0x69B1,    0x65BD,    0x6209,    0x5E15, // Band A
    0x5F9D,    0x6338,    0x6713,    0x6AAE,    0x6E89,    0x7224,    0x75BF,    0x799A, // Band B
    0x5A21,    0x562D,    0x5239,    0x4E85,    0x7D35,    0x8129,    0x851D,    0x8911, // Band E
    0x610C,    0x6500,    0x68B4,    0x6CA8,    0x709C,    0x7490,    0x7884,    0x7C38, // Band F / Airwave
    0x510A,    0x5827,    0x5F84,    0x66A1,    0x6DBE,    0x751B,    0x7C38,    0x8395  // Band C / Immersion Raceband
};

// Channels with their Mhz Values
const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Band C / Immersion Raceband
};

// do coding as simple hex value to save memory.
const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // Band A
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // Band B
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // Band E
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // Band F / Airwave
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8  // Band C / Immersion Raceband
};


char call_sign[10];
uint8_t channelIndex = 0;

// SETUP ----------------------------------------------------------------------------
void setup()
{
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(led, OUTPUT); // status pin for TV mode errors
    digitalWrite(led, HIGH);
    // Bind Switch
    pinMode(bindSwitch, INPUT);
    digitalWrite(bindSwitch, INPUT_PULLUP);

    // SPI pins for RX control
    pinMode (slaveSelectPin, OUTPUT);
    pinMode (spiDataPin, OUTPUT);
	pinMode (spiClockPin, OUTPUT);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_TUNE);
    if(eeprom_check == 255) // unused
    {
        // save 8 bit
        EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);

        // save default call sign
        strcpy(call_sign, CALL_SIGN); // load callsign
        for(uint8_t i = 0;i<sizeof(call_sign);i++) {
            EEPROM.write(EEPROM_ADR_CALLSIGN+i,call_sign[i]);
        }

    }

    // read last setting from eeprom
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);

    // load saved call sign
    for(uint8_t i = 0;i<sizeof(call_sign);i++) {
        call_sign[i] = EEPROM.read(EEPROM_ADR_CALLSIGN+i);
    }

    // Setup Done - LED ON
    digitalWrite(led, HIGH);

    // Init Display
    if (drawScreen.begin(call_sign) > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(led, !digitalRead(led));
            delay(100);
        }
    }
    // rodate the display output 180 degrees.
    // drawScreen.flip();

}

// LOOP ----------------------------------------------------------------------------
void loop()
{
}
