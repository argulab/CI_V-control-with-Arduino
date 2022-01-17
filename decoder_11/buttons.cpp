#pragma once
#include "Buttons.h"
#include "Arduino.h"

Buttons::Buttons () {};

void Buttons::init_buttons () {
  pinMode (ANTENNA_MODE_BUTTON_PIN, INPUT);
  pinMode (DATETIME_MODE_BUTTON_PIN, INPUT);
  pinMode (UP_BUTTON_PIN, INPUT);
  pinMode (DOWN_BUTTON_PIN, INPUT);
  digitalWrite (ANTENNA_MODE_BUTTON_PIN, LOW);
  digitalWrite (DATETIME_MODE_BUTTON_PIN, LOW);
  digitalWrite (UP_BUTTON_PIN, LOW);
  digitalWrite (DOWN_BUTTON_PIN, LOW);
}

bool Buttons::button_pressed (uint8_t button) {
  bool pressed = digitalRead (button);
  
  if (pressed) {
    delay (400);
    if (digitalRead (button))
      return true;
  }
  return false;
}
