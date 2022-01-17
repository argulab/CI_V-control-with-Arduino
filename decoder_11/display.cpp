#pragma once
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "Arduino.h"
#include "Display.h"
#include "Watch.h"

#define TFT_CS 35
#define TFT_DC 37

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

Display::Display () {};

void Display::init_display () {
  tft.begin ();
  tft.setRotation (3);               
  
  tft.fillScreen (ILI9341_BLUE);  
  tft.drawRect (0, 0, 320, 90, ILI9341_WHITE);
  tft.fillRect (0, 90, 320, 60, ILI9341_BLACK);
  tft.drawRect (0, 150, 320, 90, ILI9341_WHITE); 
}

void Display::clear_radio_box () {
  tft.fillRect (2, 152, 316, 86, ILI9341_BLUE); 
}

void Display::display_radio_off () {
  tft.setTextSize (4);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (76, 182);
  tft.print ("RIG OFF");
}

uint16_t frequencyYposition = 166;
uint16_t bandYposition = 200;

void Display::display_band (String band) {
  uint16_t bandXposition = 6;
  uint16_t color = band == "OUT " ? ILI9341_RED : ILI9341_WHITE;

  tft.setTextSize (3);
  tft.setTextColor (color, ILI9341_BLUE);
  tft.setCursor (bandXposition, frequencyYposition);
  tft.print (band);
}

void Display::display_megahertz (String value) {
  uint8_t megahertzXposition = 97;
  
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (megahertzXposition, frequencyYposition);
  tft.print (value + ".");
}

void Display::display_kilohertz (String value) {
  uint8_t kilohertzXposition = 151;  
  
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE); 
  tft.setCursor (kilohertzXposition, frequencyYposition);   
  tft.print (value + ",");    
}

void Display::display_hertz (String value) {
  uint8_t hertzXposition = 223;
  
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (hertzXposition, frequencyYposition);
  tft.print (value);   
}

void Display::display_antenna (String ant) {
  uint16_t antennaXposition = 98;

  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (antennaXposition, bandYposition);
  tft.print (ant);  
}

void Display::display_atu (uint8_t ant) {
  uint16_t atuXposition = 274;
  uint16_t color = ant == 1 ? ILI9341_BLACK : ILI9341_RED;
  String text = ant == 1 ? "OFF" : "ON ";

  tft.setTextSize (2);
  tft.setTextColor (ILI9341_WHITE, color);
  tft.drawRect (264, 154,54, 82, ILI9341_WHITE);
  tft.fillRect (266, 156, 50, 76, color); 
  tft.setCursor (atuXposition, frequencyYposition);
  tft.print ("ATU");
  tft.setCursor (atuXposition, bandYposition);
  tft.print (text);
}

void Display::display_antenna_mode (uint8_t mode) {
  uint16_t modeXposition = 12;
  uint16_t color = mode == 0 ? ILI9341_BLACK : ILI9341_RED;
  String text = mode == 0 ? " AUTO " : "MANUAL";

  tft.setTextSize (2);
  tft.setTextColor (ILI9341_WHITE, color);
  tft.drawRect (2, 197, 90, 40, ILI9341_WHITE);
  tft.fillRect (4, 199, 86, 36, color);
  tft.setCursor (modeXposition, bandYposition + 12);
  tft.print (text);
}

//---------------------------------------------------------------- WATCH
uint16_t utcTimeYposition = 47;
uint16_t localTimeYposition = 100;
uint16_t dateYposition = 12;

void Display::clear_datetime_box () {
  tft.fillRect (2, 2, 316, 86, ILI9341_BLUE);  
}

void Display::display_date (String dateValue) {
  uint16_t dateXpos = int ((312 - (dateValue.length () * 18)) / 2) + 6;
  tft.setTextSize (3);
  
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (6, dateYposition);
  tft.print ("                 ");
  tft.setCursor (dateXpos, dateYposition);
  tft.print (dateValue); 
}

void Display::display_utc_hour (String utcHourValue) {
  uint16_t utcTimeXposition = 62;
 
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (utcTimeXposition, utcTimeYposition);
  tft.print ("UTC = ");
  tft.print (utcHourValue);
  tft.print (":");    
}

void Display::display_local_hour (String localHourValue) {
   uint16_t localHourXposition = 40;
 
  tft.setTextSize (5);
  tft.setTextColor (ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor (localHourXposition, localTimeYposition);
  tft.print (localHourValue);
  tft.print (":");  
}

void Display::display_minute (String minuteValue) {
  uint16_t utcMinuteXposition = 224;
  uint16_t localMinuteXposition = 130;

  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (utcMinuteXposition, utcTimeYposition);
  tft.print (minuteValue); 

     
  tft.setTextSize (5);
  tft.setTextColor (ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor (localMinuteXposition, localTimeYposition);
  tft.print (minuteValue + ":");  
}

void Display::display_second (String secondValue) {
  uint8_t localSecondXposition = 220;
  
  tft.setTextSize (5);
  tft.setTextColor (ILI9341_YELLOW, ILI9341_BLACK);
  tft.setCursor (localSecondXposition, localTimeYposition);
  tft.print (secondValue);
}

//------------------------------------------------------------ SETTING DATE TIME

uint16_t IsYearXposition = 12;
uint16_t IsMonthXposition = 102;
uint16_t IsDayXposition = 156;
uint16_t IsHourXposition = 210;
uint16_t IsMinuteXposition = 264; 

void Display::display_init_setting_watch (String yr, String mt, String dy, String hr, String mn) {
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLUE);
  tft.setCursor (IsYearXposition, dateYposition);
  tft.print (yr + "/" + mt + "/" + dy + " " + hr + ":" + mn);
}

void Display::display_setting_instructions () {
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_CYAN, ILI9341_BLUE);
  tft.setCursor (IsYearXposition + 36, utcTimeYposition);
  tft.print ("UP DOWN MODE");  
}

void Display::display_save_instructions () {
  tft.setTextSize (3);
  tft.setTextColor (ILI9341_CYAN, ILI9341_BLUE);
  tft.setCursor (IsYearXposition, utcTimeYposition);
  tft.print ("UP=SAVE DOWN=NOT"); 
}

void Display::display_setting (uint8_t settingPosition, String settingValue, textcolor modeColor) {
  uint16_t color = modeColor == WHITE ? ILI9341_WHITE : ILI9341_RED;
  uint16_t settingXposition;
  
  switch (settingPosition) {
    case 0: settingXposition = IsYearXposition;   break;
    case 1: settingXposition = IsMonthXposition;  break;
    case 2: settingXposition = IsDayXposition;    break;
    case 3: settingXposition = IsHourXposition;   break;
    case 4: settingXposition = IsMinuteXposition; break;
  }
  
  tft.setTextSize (3);
  tft.setTextColor (color, ILI9341_BLUE);
  tft.setCursor (settingXposition, dateYposition);
  tft.print (settingValue); 
}
