#ifndef Radio_h
#define Radio_h
#include "Arduino.h"

#define RELAY_HEXBEAM_PIN 38
#define RELAY_APERIODIC_PIN 32
#define PIN_POWER_ON A3

//#define DEBUG_SERIAL_INPUT
//#define DEBUG_FRAME
//#define DEBUG_FREQUENCY_READ
//#define DEBUG_ACTIVE_BAND

typedef enum {
  b0, b160, b80, b60, b40, b30, b20, b17, b15, b12, b10, b6
  } bands;
  
typedef enum {
  NONE, HEXBEAM, APERIODIC
  } antennas;

typedef enum {
  AUTOMATIC, MANUAL
  } antmodes;

typedef enum {ON, OFF
  } rigstate;

class Radio {
  public:
    Radio ();
    void init_relays ();
    String formatt_2_digit (uint8_t value);
    void request_frequency ();
    bool read_frequency ();
    String megahertz_to_string (uint16_t value);
    String kilohertz_to_string (uint16_t value);
    String hertz_to_string (uint8_t value);
    uint8_t decode_band (uint32_t frq);
    String band_to_name (uint8_t band);
    uint8_t decode_antenna (uint8_t band);
    String antenna_to_name (uint8_t ant);
    uint16_t read_analog_port (uint8_t pin);
    uint32_t decode_frequency ();
    void debug_frame ();
    void set_transceive_mode_off ();
    void set_date (uint16_t nowYear, uint8_t nowMonth, uint8_t nowDay);
    void set_time (uint8_t nowHour, uint8_t nowMinute);
    rigstate read_power_state();
  private:
    uint8_t hexadecimal_to_decimal (uint8_t hexadecimalValue); 
    uint8_t decimal_to_hexadecimal (uint8_t decimalValue);  
};
#endif
