#pragma once
#include <RTClib.h>
#include "Arduino.h"
#include "Watch.h"
#include "Display.h"
#include "Buttons.h"

//#define DEBUG_WATCH

uint8_t PreviousDay = 99;
uint8_t PreviousHour = 99;
uint8_t PreviousMinute = 99;

RTC_DS3231 rtc; 

Display dis = Display ();
Buttons but = Buttons ();

Watch::Watch() {
  _rtc = rtc;
}

void Watch::init() {
  _rtc.begin();
}

DateTime Watch::now() {
  return _rtc.now();
}

//------------------------------------------------------ LOCAL METHODS

String Watch::int_to_string (uint8_t value) {
  if (value == 0)
    return "00";
  if (value < 10)
    return "0" + String (value);
  return String (value);
}

bool Watch::leap_year (uint16_t yearValue) {
  return (yearValue % 4 == 0) && (yearValue % 10 != 0);
}

String Watch::name_of_month (months nowMonth) {
  switch (nowMonth) {
    case jan: return "ENE";
    case feb: return "FEB";
    case mar: return "MAR";
    case apr: return "ABR";
    case may: return "MAY";
    case jun: return "JUN";
    case jul: return "JUL";
    case aug: return "AGO";
    case sep: return "SEP";
    case oct: return "OCT";
    case nov: return "NOV";
    case dec: return "DIC";
  }
}

String Watch::name_of_day_of_week (dow nowDow) {
  switch (nowDow) {
    case sun: return "DOMINGO";
    case mon: return "LUNE";
    case tue: return "MARTES";
    case wed: return "MIERCOLES";
    case thu: return "JUEVES";
    case fri: return "VIERNES";
    case sat: return "SABADO";
  }
}

uint16_t Watch::spanish_timezone_offset (months dtMonth, uint8_t dtDay, dow dtDow, uint8_t dtHour) {
  uint8_t lastDay = 24;
  uint8_t hourOfChange = 3;
  uint16_t summerOffset = 7200;
  uint16_t winterOffset = 3600;

  switch (dtMonth) {
    case mar:
      if (dtDay <= lastDay) return winterOffset;
      if (dtDow == sun) return (dtHour <= hourOfChange) ? winterOffset : summerOffset;
      return winterOffset;
      
    case oct:
      if (dtDay <= lastDay) return summerOffset;
      if (dtDow == sun) return (dtHour <= hourOfChange) ? summerOffset : winterOffset;
      return summerOffset;
      
    default:
      if (dtMonth > mar && dtMonth < oct) return summerOffset;
      if (dtMonth < mar || dtMonth > oct) return winterOffset;   
  }
}

//---------------------------------------------------------------- WATCH METHODS

DateTime Watch::datetime_utc_time_calc (DateTime localNow) {
  uint32_t offsetTime = spanish_timezone_offset (
    localNow.month (), localNow.day (), localNow.dayOfTheWeek (), localNow.hour ());  
  return localNow.unixtime () - offsetTime;
}

void Watch::init_watch () {
  DateTime datetimeLocalNow = now ();
  DateTime datetimeUtcNow = datetime_utc_time_calc (datetimeLocalNow);

  #ifdef DEBUG_WATCH
    Serial.print ("Hora UTC = ");
    Serial.println (datetimeUtcNow.hour ());
    Serial.print ("Hora Local = ");
    Serial.println (datetimeLocalNow.hour ());
    Serial.print ("Offset time = ");
    Serial.println (offsetTime);
  #endif  /DEBUG_WATCH
    
  String formattedDate = String (datetimeLocalNow.day ()) + " " +
    name_of_month (datetimeLocalNow.month ()) + " " + 
    name_of_day_of_week (datetimeLocalNow.dayOfTheWeek ());
  dis.display_date (formattedDate);

  dis.display_utc_hour (int_to_string (datetimeUtcNow.hour ()));
  dis.display_local_hour (int_to_string (datetimeLocalNow.hour ()));
  dis.display_minute (int_to_string (datetimeUtcNow.minute ()));  
  
  PreviousDay = datetimeLocalNow.day ();
  PreviousHour = datetimeUtcNow.hour ();
  PreviousMinute = datetimeUtcNow.minute ();
}

void Watch::service_datetime () {
  DateTime datetimeLocalNow = now ();
  DateTime datetimeUtcNow = datetime_utc_time_calc (datetimeLocalNow); 

  if (datetimeUtcNow.minute () != PreviousMinute) {
    PreviousMinute = datetimeUtcNow.minute ();    
    dis.display_minute (int_to_string (datetimeUtcNow.minute ()));
  }

  if (datetimeUtcNow.hour () != PreviousHour) {
    PreviousHour = datetimeUtcNow.hour ();
    dis.display_utc_hour (int_to_string (datetimeUtcNow.hour ()));
    dis.display_local_hour (int_to_string (datetimeLocalNow.hour ()));
  }

  dis.display_second (int_to_string (datetimeUtcNow.second ()));
}

int Watch::check_updown_buttons () {
  if (but.button_pressed (UP_BUTTON_PIN))
    return 1;
  if (but.button_pressed (DOWN_BUTTON_PIN))
    return -1;
  return 0;
}

//------------------------------------------------------------ SET DATE TIME 

uint16_t DatetimeSettings [5];          //Primero creo un array público con los datos del RTC

void Watch::read_datetime_settings () {
  DateTime datetimeNow = now ();
  DatetimeSettings [YEAR]   = datetimeNow.year ();
  DatetimeSettings [MONTH]  = datetimeNow.month ();
  DatetimeSettings [DAY]    = datetimeNow.day ();
  DatetimeSettings [HOUR]   = datetimeNow.hour ();
  DatetimeSettings [MINUTE] = datetimeNow.minute ();
}

uint8_t Watch::on_datetime_button_mode_pressed (uint8_t setValue) {   
  switch (setValue) {
    case YEAR:       //YEAR first parameter for setup, the display data is String (YEAR);
      dis.display_setting (setValue, String (DatetimeSettings [setValue]), WHITE);
      dis.display_setting (setValue + 1, int_to_string (DatetimeSettings [setValue + 1]), RED);
      return MONTH;
      
    case MINUTE:    //MINUTE last parameter for setup
      dis.display_setting (setValue, int_to_string (DatetimeSettings [setValue]), WHITE);
      return OUT;
      
    default:
      dis.display_setting (setValue, int_to_string (DatetimeSettings [setValue]), WHITE);
      dis.display_setting (setValue + 1, int_to_string (DatetimeSettings [setValue + 1]), RED); 

      if (setValue == MONTH)  return DAY;
      if (setValue == DAY)    return HOUR;
      if (setValue == HOUR)   return MINUTE;  
  }
}

uint8_t Watch::calc_day_of_month_upper_bound () {
  if (DatetimeSettings [MONTH] == feb) 
    return (DatetimeSettings [YEAR] % 4 == 0) && (DatetimeSettings [YEAR] % 10 != 0) ? 29 : 28;
     
  if (
    DatetimeSettings [MONTH] == apr || 
    DatetimeSettings [MONTH] == jun || 
    DatetimeSettings [MONTH] == sep || 
    DatetimeSettings [MONTH] == nov
    )
    
    return 30; 
  return 31;
}

void Watch::on_finish_read_settings () {
  dis.display_save_instructions ();
  
  while (true) {
    if (but.button_pressed (DOWN_BUTTON_PIN)) 
      return;
    if (but.button_pressed (UP_BUTTON_PIN)) {
      _rtc.adjust (DateTime (
        DatetimeSettings [0],
        DatetimeSettings [1],
        DatetimeSettings [2],
        DatetimeSettings [3],
        DatetimeSettings [4],
        0));
      delay (200); 
      return; 
    }
  }
}

void Watch::calc_new_setting_value (uint8_t settingType, int settingAdd) {
  if (settingType == YEAR) {
    if (settingAdd == 1)
      DatetimeSettings [YEAR] += 1;
    if (settingAdd == -1) 
      DatetimeSettings [YEAR] -= 1;
    return;
  }

  if (settingType == MONTH) {    
    if (settingAdd == 1) 
      DatetimeSettings [MONTH] = DatetimeSettings [MONTH] == 12 ? 1 : DatetimeSettings [MONTH] += 1;
    if (settingAdd == -1)
      DatetimeSettings [MONTH] = DatetimeSettings [MONTH] == 1 ? 12 : DatetimeSettings [MONTH] -= 1;   
    return;
  }

  if (settingType == DAY) {
    uint8_t upperBound = calc_day_of_month_upper_bound ();     
    if (settingAdd == 1) 
      DatetimeSettings [DAY] = DatetimeSettings [DAY] == upperBound ? 1 : DatetimeSettings [DAY] += 1;   
    if (settingAdd == -1)
      DatetimeSettings [DAY] = DatetimeSettings [DAY] == 1 ? upperBound : DatetimeSettings [DAY] -= 1;      
    return;
  }

  if (settingType == HOUR) {
    if (settingAdd == 1)
      DatetimeSettings [HOUR] = DatetimeSettings [HOUR] == 23 ? 0 : DatetimeSettings [HOUR] += 1;
    if (settingAdd == -1)
      DatetimeSettings [HOUR] = DatetimeSettings [HOUR] == 0 ? 23 : DatetimeSettings [HOUR] -= 1;
    return;    
  }

  if (settingType == MINUTE) {
    if (settingAdd == 1) 
      DatetimeSettings [MINUTE] = DatetimeSettings [MINUTE] == 59 ? 0 : DatetimeSettings [MINUTE] += 1;
    if (settingAdd == -1)
      DatetimeSettings [MINUTE] = DatetimeSettings [MINUTE] == 0 ? 59 : DatetimeSettings [MINUTE] -= 1;
    return;
  }
}

bool Watch::set_datetime_service () {
  uint8_t setting = YEAR;
   
  read_datetime_settings ();            //Read the data of the RTC and set the DatatimeSettings [5] array
  dis.clear_datetime_box ();            
  dis.display_init_setting_watch (      
    String (DatetimeSettings [YEAR]), 
    int_to_string (DatetimeSettings [MONTH]), 
    int_to_string (DatetimeSettings [DAY]), 
    int_to_string (DatetimeSettings [HOUR]), 
    int_to_string (DatetimeSettings [MINUTE])); 
    
  dis.display_setting_instructions ();
  dis.display_setting (setting, String (DatetimeSettings [setting]), RED); 

  while (true) {
    int setValue = 0;
    
    if (but.button_pressed (DATETIME_MODE_BUTTON_PIN))
      setting = on_datetime_button_mode_pressed (setting);

    if (setting == OUT)
      break;

    if (but.button_pressed (UP_BUTTON_PIN))
      setValue = 1;

    if (but.button_pressed (DOWN_BUTTON_PIN))
      setValue = -1;

    #ifdef DEBUG_SETTING_DATA
      if (setValue != 0) {
        Serial.print ("Setting actual = ");
        Serial.println (setting);
        Serial.print ("Valor = ");
        Serial.println (DatetimeSettings [setting]);
        Serial.print ("Valor adicion = ");
        Serial.println (setValue);
        Serial.print ("Nuevo Valor = ");
        Serial.println (newValue);
      }
    #endif  //DEBUG_SETTING_DATA

    if (setValue != 0) {
      calc_new_setting_value (setting, setValue);
      String displayDatetime = setting == 0 ? String (DatetimeSettings [setting]) : int_to_string (DatetimeSettings [setting]);
      dis.display_setting (setting, displayDatetime, RED);
    }
  }

  on_finish_read_settings ();
  dis.clear_datetime_box ();
  init_watch ();
  return true; 
}
