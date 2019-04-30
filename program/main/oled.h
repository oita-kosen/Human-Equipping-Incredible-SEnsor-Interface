#ifndef _OLED_H_
#define _OLED_H_

#include <Adafruit_SSD1306.h>
#include "logo.h"

const byte SSD1306_ADDR = 0x3C;
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int LOGO_WIDTH = 64;
const int LOGO_HEIGHT = 64;

void setupOLED()
{
    display.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDR);
}

void clearDisplay()
{
    display.clearDisplay();
    display.display();
}

void drawHeisei()
{
    display.clearDisplay();

    display.drawBitmap(
    0,
    0,
    logo_Hei, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.drawBitmap(
    64,
    0,
    logo_Sei, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.display();
}

void drawMigi()
{
    display.clearDisplay();

    display.drawBitmap(
    32,
    0,
    logo_Migi, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
}

void drawHidari()
{
    display.clearDisplay();

    display.drawBitmap(
    32,
    0,
    logo_Hidari, LOGO_WIDTH, LOGO_HEIGHT, 1);
    display.display();
}

void drawReiWa()
{
    display.clearDisplay();

    display.drawBitmap(
    0,
    0,
    logo_Rei, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.drawBitmap(
    64,
    0,
    logo_Wa, LOGO_WIDTH, LOGO_HEIGHT, 1);

    display.display();
}

#endif