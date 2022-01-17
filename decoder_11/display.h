#ifndef Display_h
#define Display_h

#include "Watch.h"

class Display {
  public:
    Display ();
    void init_display ();   
    void clear_radio_box ();
    void display_radio_off ();
    void display_megahertz (String value);
    void display_kilohertz (String value);
    void display_hertz (String value);
    void display_band (String band);
    void display_antenna (String ant);
    void display_atu (uint8_t ant);
    void display_antenna_mode (uint8_t mode);

    void clear_datetime_box ();
    void display_date (String dateValue);
    void display_utc_hour (String utcHourValue);
    void display_local_hour (String localHourValue);
    void display_minute (String minuteValue);
    void display_second (String secondValue);

    void display_init_setting_watch (String yr, String mt, String dy, String hr, String mn);
    void display_setting_instructions ();
    void display_save_instructions ();
    void display_setting (uint8_t settingPosition, String settingValue, textcolor modeColor);
    
  private:
};
#endif
