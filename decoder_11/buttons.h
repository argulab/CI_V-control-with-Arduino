#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"
#include "Watch.h"

#define DATETIME_MODE_BUTTON_PIN 7
#define ANTENNA_MODE_BUTTON_PIN 9
#define UP_BUTTON_PIN 5
#define DOWN_BUTTON_PIN 3

class Buttons {
  public:
    Buttons ();
    void init_buttons ();
    bool button_pressed (uint8_t button);
  private:
};
#endif
