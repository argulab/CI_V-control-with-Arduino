#ifndef Watch_h
#define Watch_h

#include <RTClib.h>
#include "Arduino.h"

//#define DEBUG_SETTING_DATA
//#define DEBUG_SETTING_DAY
//#define DEBUG_BOUNDS

typedef enum {
  jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec
  } months;
  
typedef enum {
  sun, mon, tue, wed, thu, fri, sat
  } dow;

typedef enum {
  UTC, LOCAL
  } timetable;

typedef enum {
  YEAR, MONTH, DAY, HOUR, MINUTE, OUT
  } datetime;

typedef enum {
  NORMAL, SETTING
  } datetimemodes;

typedef enum {
  WHITE, RED   
  } textcolor;

class Watch {
  public:
    Watch ();
    void init ();
    DateTime now ();
    void init_watch ();
    void init_datetime_buttons ();
    void set_datetime ();
    void service_datetime (); 
    bool set_datetime_service ();  
  private:
    RTC_DS3231 _rtc;
    String int_to_string (uint8_t value);
    bool leap_year (uint16_t yearValue);
    String name_of_month (months nowMonth);
    String name_of_day_of_week (dow nowDow);
    uint16_t spanish_timezone_offset (months dtMonth, uint8_t dtDay, dow dtDow, uint8_t dtHour);
    DateTime datetime_utc_time_calc (DateTime localNow);
    int check_updown_buttons ();
    void read_datetime_settings ();
    uint8_t on_datetime_button_mode_pressed (uint8_t setValue);    
    uint16_t calc_setting (uint8_t st, uint16_t stValue, int stAdd, uint8_t mt, uint16_t yr);
    uint8_t calc_day_of_month_upper_bound ();
    uint8_t check_setting_bounds (uint8_t value, uint8_t upper, uint8_t lower);
    void on_finish_read_settings ();
    void calc_new_setting_value (uint8_t settingType, int settingAdd);
};
#endif  
