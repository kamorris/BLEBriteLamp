/*
Copyright (c) Kate Morris, 2015.

Uses Bluetooth Low Energy and Arduino to drive a chain of Shiftbrites - see http://docs.macetech.com/doku.php/shiftbrite_2.0
Earlier generation Shiftbrites also work, and mixed chains are possible.  Works with the BriteLamp iOS app, soon to be available on the AppStore (free).

Required:

1) ShiftBrite library from https://github.com/Phara0h/ShiftBrite

2) Nordic Semiconductor library from https://github.com/NordicSemiconductor/ble-sdk-arduino

3) BLE library from https://github.com/RedBearLab/nRF8001

The code below is based on code provided by RedBearLab.  Their original copyright notice is below.

Portions Copyright (c) 2012, 2013 RedBearLab
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software 
 without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */

#include <ShiftBrite.h>
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>

#define PROTOCOL_MAJOR_VERSION   0 //
#define PROTOCOL_MINOR_VERSION   1 //
#define PROTOCOL_BUGFIX_VERSION  2 // bugfix

// Note: the blend micro uses pins 6 and 7 for bluetooth; the blend uses 8 / 9.  Adjust these as needed.
// standard colors on the Shiftbrite cables are D: white, L: blue, E: green, C: yellow.
#define DATA_PIN    9
#define LATCH_PIN  10
#define ENABLE_PIN 11
#define CLOCK_PIN  12
// override the default REQN and RDYN (new for minor version 1)
#define REQN		6
#define RDYN 		7

#define MAX_COLORS 20
#define RED_IDX   0
#define GREEN_IDX 1
#define BLUE_IDX  2

ShiftBrite shift(DATA_PIN, LATCH_PIN, ENABLE_PIN, CLOCK_PIN);
byte colors[MAX_COLORS][3];  // longest chain
int delayMillis;
int last = 0;
int lightCount;
int currentColor;

void setup()
{
  Serial.begin(57600);
  Serial.println("BLE BriteLamp");

  ble_set_name("BriteLamp");
  currentColor = -1;
  lightCount = 0;
  ble_set_pins(REQN, RDYN);
  // Init. and start BLE library.
  ble_begin();
  Serial.println("Starting "); 
}

static byte buf_len = 0;

void ble_write_string(byte *bytes, uint8_t len)
{
  if (buf_len + len > 20)
  {
    for (int j = 0; j < 15000; j++)
      ble_do_events();

    buf_len = 0;
  }

  for (int j = 0; j < len; j++)
  {
    ble_write(bytes[j]);
    buf_len++;
  }

  if (buf_len == 20)
  {
    for (int j = 0; j < 15000; j++)
      ble_do_events();

    buf_len = 0;
  }  
}

void readColors(int start, int readCount) {
  for (byte i = start; i < readCount + start; ++i) {
    colors[i][RED_IDX]   = ble_read();
    colors[i][GREEN_IDX] = ble_read();
    colors[i][BLUE_IDX]  = ble_read();
  }
}
void loop()
{
  if(ble_available())
  {
    byte cmd;
    cmd = ble_read();
    Serial.write(cmd);

    // Parse data here
    switch (cmd)
    {
    case 'V': // query protocol version
      {
        byte buf[] = {
          'V', 0x00, 0x00, 0x01                };
        ble_write_string(buf, 4);
      }
      break;
    case 'C': 
      {
        // continuation
        unsigned int readCount = (unsigned int)ble_read();  // byte is the number of lights  Needs to be <= 5, empirically
        if (readCount + lightCount >= MAX_COLORS) {
          Serial.println("too many colors!");
          break;
        }
        readColors(lightCount, readCount);
        lightCount += readCount;
      }
      break;

    case 'L':  // reading the command string.
      {
        lightCount = (unsigned int)ble_read();  // byte is the number of lights  Needs to be <= 5, empirically?
        if (lightCount >= MAX_COLORS) {
          Serial.println("too many colors!");
          break;
        }
        byte speed = ble_read();
        if (speed == 0) {
          delayMillis = 1000;  // millisecond delay; default 1 second
        }
        else {
          delayMillis = speed * 4;
        }
        currentColor = 0;
        readColors(0, lightCount);
        break;
      }
      default:
      Serial.print(cmd); Serial.println(" was read, and I don't know what to do");  // should never see this message.
    }

    // send out any outstanding data
    ble_do_events();
    buf_len = 0;

    return; // only do this task in this loop
  }

  // no input or commands, so go ahead and send colors
  // just set the first one.
  // ShiftBrite.
  if (millis() - last > delayMillis) 
  {
    last = millis();
    if (currentColor != -1) {
      if (currentColor == lightCount)
        currentColor = 0;

     shift.sendColor((unsigned long)colors[currentColor][RED_IDX], (unsigned long)colors[currentColor][GREEN_IDX], (unsigned long)colors[currentColor][BLUE_IDX]);
     currentColor++;
    }
  }

  // No input data, no commands, process analog data
  if (!ble_connected()) {
    ble_do_events();
    return;  
  }

  ble_do_events();
  buf_len = 0;
}



