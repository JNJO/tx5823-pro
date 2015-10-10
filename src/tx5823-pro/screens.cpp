/*
 * OLED Screens by Shea Ivey

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

#include "settings.h"

#include "screens.h" // function headers
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

// New version of PSTR that uses a temp buffer and returns char *
// by Shea Ivey
#define PSTR2(x) PSTRtoBuffer_P(PSTR(x))
char PSTR2_BUFFER[30]; // adjust size depending on need.
char *PSTRtoBuffer_P(PGM_P str) { uint8_t c='\0', i=0; for(; (c = pgm_read_byte(str)) && i < sizeof(PSTR2_BUFFER); str++, i++) PSTR2_BUFFER[i]=c;PSTR2_BUFFER[i]=c; return PSTR2_BUFFER;}

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if !defined SSD1306_128_32
    #error("Screen size incorrect, please fix Adafruit_SSD1306.h!");
#endif

bool blinker = true;

screens::screens() {
}

char screens::begin(const char *call_sign) {
    // Set the address of your OLED Display.
    // 128x32 ONLY!!
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
    // init done
    reset();
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(PSTR2("Booting up..."));

    display.display();
    display.setCursor(((display.width() - (strlen(call_sign)*12)) / 2),8*1+4);
    display.setTextSize(2);
    display.print(call_sign);
    display.display();
    delay(2000);
    return 0; // no errors
}

void screens::reset() {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void screens::flip() {
    display.setRotation(3);
}


void screens::updateFrequencyInformation(uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    reset();
    display.setTextSize(1);
    display.setCursor(48,0);
    display.print(call_sign);
    display.setTextSize(2);
    display.setCursor(48,8);
    display.setTextColor(WHITE);
    display.print(channelFrequency);
    display.setCursor(48+(12*4),15);
    display.setTextSize(1);
    display.print("Ghz");
    display.drawPixel(48+(8*1+3),21, WHITE);

    display.setTextSize(4);
    display.setTextColor(WHITE);
    display.setCursor(0,2);
    display.print(channelName, HEX);
    display.setTextSize(1);
}

void screens::updateStatus(const char *status) {
    updateStatus(status, false, true);
}

void screens::updateStatus(const char *status, bool invert) {
    updateStatus(status, invert, true);
}

void screens::updateStatus(const char *status, bool invert, bool blink) {
    if(blink) {
        blinker = !blinker;
    }
    else {
        blinker = invert ? false : true;
    }

    // blank out status region
    display.fillRect(48, display.height()-9, display.width()-48, 9, invert ? WHITE : BLACK);

    display.setTextSize(1);
    display.setCursor(48+(invert ? ((((display.width()-48) - (strlen(status)*6)) / 2)) : 0),24); // center text
    display.setTextColor(blinker ? WHITE : BLACK);
    display.print(status);
}

void screens::bindMode(uint8_t state, uint8_t channelName, uint16_t channelFrequency, const char *call_sign, bool force_redraw){
    if(force_redraw) {
        updateFrequencyInformation(channelName, channelFrequency, call_sign);
    }
    switch(state) {
        case STATE_BIND_MODE:
        case STATE_BIND_MODE_WAITING:
            updateStatus("BINDING",true);
            break;
        case STATE_BIND_MODE_RECEIVED:
            // received good data
            updateStatus("RECEIVED",true,false);
            break;
        case STATE_BIND_MODE_FAILED:
            break;
    }
    display.display();
}

// SCREEN SAVER
void screens::screenSaver(uint8_t channelName, uint16_t channelFrequency, const char *call_sign, bool force_redraw){
    if(force_redraw) {
        updateFrequencyInformation(channelName, channelFrequency, call_sign);
    }
    updateStatus("TRANSMITTING");
    display.display();
}
