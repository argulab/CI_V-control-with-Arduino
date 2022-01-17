#pragma once
#include "Arduino.h"
#include "Radio.h"
#include "Watch.h"

const uint8_t SOFrame = 0xFE; 
const uint8_t RIGaddress = 0x94;
const uint8_t CONTROLaddress = 0xE0;
const uint8_t COMMANDfrequency = 0x03;
const uint8_t EOFrame = 0xFD; 

const uint8_t SOFposition = 0;
const uint8_t COMMANDposition = 4;
const uint8_t DATA1position = 5;
const uint8_t DATA2position = 6;
const uint8_t DATA3position = 7;
const uint8_t DATA4position = 8;
const uint8_t DATA5position = 9;
const uint8_t EOFposition = 10;

uint8_t RelayPins [] = {32, 34, 36, 38, 40, 42, 44, 46};
uint8_t FrequencyFrame [11] ;
uint8_t FramePos = 0;

Radio::Radio () {};

void Radio::debug_frame () {
  for (uint8_t i = 0; i <= 10; i++) {
    Serial.print ("Frame pos = ");
    Serial.print (i);
    Serial.print (" value = ");
    Serial.println (FrequencyFrame [i], HEX);    
  }
}

void Radio::init_relays () {
  uint8_t maxRelay = 7;
    
  for (uint8_t relayNum = 0; relayNum <= maxRelay; relayNum++) {
    uint8_t relayPin = RelayPins [relayNum];
    pinMode (relayPin, OUTPUT);
    digitalWrite (relayPin, LOW);
  }
}

uint8_t Radio::hexadecimal_to_decimal (uint8_t hexadecimalValue) {
  return ((hexadecimalValue >> 4) * 10) + (hexadecimalValue & 0x0F);
}

uint8_t Radio::decimal_to_hexadecimal (uint8_t decimalValue) {
  return ((decimalValue/10) *16) + (decimalValue % 10);
}

String Radio::formatt_2_digit (uint8_t value) {
  if (value == 0)
    return "00";
  return value < 10 ? "0" + String (value) : String (value);  
}

void Radio::request_frequency () {
  Serial1.flush ();
  Serial1.write (SOFrame); 
  Serial1.write (SOFrame); 
  Serial1.write (RIGaddress); 
  Serial1.write (CONTROLaddress);
  Serial1.write (COMMANDfrequency); 
  Serial1.write (EOFrame); 
  delay (20); 
}

void Radio::set_transceive_mode_off () {
  Serial1.flush ();
  Serial1.write (0xFE);   //Start of Frame
  Serial1.write (0xFE);   //Start of Frame
  Serial1.write (0x94);   //Rig address
  Serial1.write (0xE0);   //Control address
  Serial1.write (0x1A);   //Command
  Serial1.write (0x05);   //subcommand
  Serial1.write (0x00);   //Subcommand
  Serial1.write (0x71);   //Subcommand
  Serial1.write (0x00);   //Data
  Serial1.write (0xFD);   //End of Frame
  delay (20);  
}

void Radio::set_date (uint16_t nowYear, uint8_t nowMonth, uint8_t nowDay) {
  Serial1.flush();
  Serial1.write(0xFE); 
  Serial1.write(0xFE); 
  Serial1.write(0x94); 
  Serial1.write(0xE0);
  Serial1.write(0x1A); 
  Serial1.write(0x05);
  Serial1.write(0x00);
  Serial1.write(0x94);
  Serial1.write(decimal_to_hexadecimal(int(nowYear/100)));
  Serial1.write(decimal_to_hexadecimal(nowYear % 100));
  Serial1.write(decimal_to_hexadecimal(nowMonth));
  Serial1.write(decimal_to_hexadecimal(nowDay));
  Serial1.write(0xFD); 
  delay(20);  
}

void Radio::set_time (uint8_t nowHour, uint8_t nowMinute) {
  Serial1.flush();
  Serial1.write(0xFE); 
  Serial1.write(0xFE); 
  Serial1.write(0x94); 
  Serial1.write(0xE0);
  Serial1.write(0x1A); 
  Serial1.write(0x05);
  Serial1.write(0x00);
  Serial1.write(0x95);
  Serial1.write(decimal_to_hexadecimal (nowHour));
  Serial1.write(decimal_to_hexadecimal (nowMinute));
  Serial1.write(0xFD); 
  delay(20);  
}

String Radio::megahertz_to_string (uint16_t value) {
  return value < 10 ? " " + String (value) : String (value);
}

String Radio::kilohertz_to_string (uint16_t value) {
  if (value == 0)
    return "000";
  if (value < 100 && value >= 10)
    return "0" + String (value);
  return value < 10 ? "00" + String (value) : String (value);  
}

String Radio::hertz_to_string (uint8_t value) {
  if (value == 0)
    return "00";
  return value < 10 ? "0" + String (value) : String (value);   
}

uint32_t Radio::decode_frequency () {
  uint32_t frq = 0;

  for (uint8_t bytePos = SOFposition; bytePos <= EOFposition; bytePos++) {
    uint32_t hexValue = FrequencyFrame [bytePos];
    uint32_t decValue = hexadecimal_to_decimal (hexValue);
  
    switch (bytePos) {
      case DATA1position:
        frq += int (decValue / 10); break;
      case DATA2position:
        frq += decValue * 10; break;
      case DATA3position:
        frq += decValue * 1000; break;
      case DATA4position:
        frq += decValue * 100000; break;
      case DATA5position:
        frq += decValue * 10000000; break;
    }
  }  
  return frq; 
}

uint8_t Radio::decode_band (uint32_t frq) {
  if (frq >=  181000 && frq <=  200000) return 1;
  if (frq >=  350000 && frq <=  380000) return 2;
  if (frq >=  535100 && frq <=  536600) return 3;
  if (frq >=  700000 && frq <=  720000) return 4;
  if (frq >= 1010000 && frq <= 1015000) return 5;
  if (frq >= 1400000 && frq <= 1435000) return 6;
  if (frq >= 1806800 && frq <= 1816800) return 7;
  if (frq >= 2100000 && frq <= 2145000) return 8;
  if (frq >= 2489000 && frq <= 2499000) return 9;
  if (frq >= 2800000 && frq <= 2970000) return 10;
  if (frq >= 5000000 && frq <= 5200000) return 11;
  return 0;
}

String Radio::band_to_name (uint8_t bandNumber) {   
  switch (bandNumber) {
    case  0: return "OUT ";
    case  1: return "160m";
    case  2: return " 80m";
    case  3: return " 60m";
    case  4: return " 40m";
    case  5: return " 30m";
    case  6: return " 20m";
    case  7: return " 17m";
    case  8: return " 15m";
    case  9: return " 12m";
    case 10: return " 10m";
    case 11: return "  6m";
  }
}

uint8_t Radio::decode_antenna (uint8_t band) {
  uint8_t hex = 1;
  uint8_t ape = 2;
  
  if (band == 1 || band == 2 || band == 3 || band == 4 || band == 5)
    return ape;
  if (band == 6 || band == 7 || band == 8 || band == 9 || band == 10 || band == 11)
    return hex;
  return 0;
}

String Radio::antenna_to_name (uint8_t ant) {
  switch (ant) {
    case 1: return "HEXBEAM  ";
    case 2: return "APERIODIC";
  }
}

bool Radio::read_frequency () {
  while (Serial1.available ()) {
    uint8_t incomingByte = Serial1.read ();
    
    #ifdef DEBUG_SERIAL_INPUT
      Serial.print ("Byte Frame = ");
      Serial.print (FramePos);
      Serial.print (" Byte value = ");
      Serial.println (incomingByte, HEX);
    #endif  //DEBUG_SERIAL_INPUT
    
    FrequencyFrame [FramePos] = incomingByte;
    FramePos++;
    bool endOfFrame = FramePos == 11;

    if (incomingByte == EOFrame || endOfFrame) {
      FramePos = 0;
      if (incomingByte == EOFrame && endOfFrame)
        return true;
    }
  } 
}

uint16_t Radio::read_analog_port (uint8_t pin) {
  uint32_t rawSumma = 0;
  uint16_t rawRead = 0;
  for (uint8_t i = 0; i < 100; i++) {
    rawSumma += analogRead(pin);
  }
  return int (rawSumma / 100);
}

rigstate Radio::read_power_state() {
  return read_analog_port(PIN_POWER_ON) > 600 ? ON : OFF;
}
