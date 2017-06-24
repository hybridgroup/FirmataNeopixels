#include <ConfigurableFirmata.h>
#include "FirmataNeopixels.h"

FirmataNeopixels::FirmataNeopixels()
{
   pixelCount = DEFAULT_NUMPIXELS;
   pixelPin = DEFAULT_PIN;
   colorDepth = 3;
}

// FirmataFeature interface functions. Required for any Firmata plugin.

boolean FirmataNeopixels::handlePinMode(byte pin, int mode)
{
    return false;
}

void FirmataNeopixels::handleCapability(byte pin)
{
}

boolean FirmataNeopixels::handleSysex(byte command, byte argc, byte *argv)
{
    if (command == NEOPIXEL_CMD)
    {
        byte pixelCommand;
        pixelCommand = argv[0];

        if (pixelCommand == PIXEL_OFF)
        {
            turnOff();
            return true;
        }

        if (pixelCommand == PIXEL_CONFIG)
        {
           uint8_t pin = (uint8_t)argv[1];
           uint16_t len = (uint16_t)argv[2] + ((uint16_t)argv[3]<<7);

           setConfig(pin, len);
           return true;
        }

        if (pixelCommand == PIXEL_SHOW)
        {
            show();
            return true;
        }

        if (pixelCommand == PIXEL_SET_PIXEL)
        {
            uint16_t i = (uint16_t)argv[1] + ((uint16_t)argv[2]<<7);
            uint32_t color = (uint32_t)argv[3] + ((uint32_t)argv[4]<<7) +
                ((uint32_t)argv[5]<<14) + ((uint32_t)argv[6] << 21);

            setPixel(i, color);
            return true;
        }
    }

    return false;
}

void FirmataNeopixels::reset()
{
   pixels = Adafruit_NeoPixel(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);
   pixels.begin();
}

// FirmataNeopixels interface functions

void FirmataNeopixels::turnOff()
{
   pixels.clear();
   pixels.show();

   Firmata.write(START_SYSEX);
   Firmata.write(NEOPIXEL_CMD);
   Firmata.write(PIXEL_OFF);
   Firmata.write(END_SYSEX);
}

void FirmataNeopixels::show()
{
   pixels.show();

   Firmata.write(START_SYSEX);
   Firmata.write(NEOPIXEL_CMD);
   Firmata.write(PIXEL_SHOW);
   Firmata.write(END_SYSEX);
}

void FirmataNeopixels::setPixel(uint16_t i, uint32_t color) {
   pixels.setPixelColor(i, color);

   Firmata.write(START_SYSEX);
   Firmata.write(NEOPIXEL_CMD);
   Firmata.write(PIXEL_SET_PIXEL);
   Firmata.write(END_SYSEX);
}

void FirmataNeopixels::setConfig(uint8_t pin, uint16_t len) {
   pixelPin = pin;
   pixelCount = len;

   reset();

   Firmata.write(START_SYSEX);
   Firmata.write(NEOPIXEL_CMD);
   Firmata.write(PIXEL_CONFIG);
   Firmata.write(END_SYSEX);
}
