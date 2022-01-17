#include "Radio.h"
#include "Display.h"
#include "Watch.h"
#include "Buttons.h"

//--------------------------------------- INITIAL PARAMETERS

bands PreviousBand = b0;
antmodes AntennaMode = AUTOMATIC;
datetimemodes DatetimeMode = NORMAL;
antennas PreviousAntenna = NONE;
rigstate PreviousRigState = OFF;

uint8_t  PreviousMegahertz = 99;
uint16_t PreviousKilohertz = 9999;
bool RadioOn = false;
uint32_t InitFrequencyReadDelay = millis ();
uint32_t InitDatetimeReadDelay = millis ();

//------------------------------------------------------ CLASS CONSTRUCTORS

Display dsp = Display ();
Radio rig = Radio ();
Watch wtc = Watch ();
Buttons btn = Buttons ();

//------------------------------------------------------ RADIO METHODS

void restore_initial_radio_parameters () {
  PreviousMegahertz = 99;
  PreviousKilohertz = 9999;
  PreviousAntenna = NONE; 
  PreviousBand = b0; 
  AntennaMode = AUTOMATIC;
}

void service_frequency (uint32_t frq) {
  uint16_t megaHertz = int (frq / 100000);
  uint16_t kiloHertz = int (frq / 100) - (megaHertz * 1000);
  uint16_t hertz = frq % 100;

  if (megaHertz != PreviousMegahertz) {
    PreviousMegahertz = megaHertz;
    String formattedMegahertz = rig.megahertz_to_string (megaHertz); 
    dsp.display_megahertz (formattedMegahertz);
  }
  
  if (kiloHertz != PreviousKilohertz) {
    PreviousKilohertz = kiloHertz;
    String formattedKilohertz = rig.kilohertz_to_string (kiloHertz);
    dsp.display_kilohertz (formattedKilohertz);
  }
  
  String formattedHertz = rig.hertz_to_string (hertz);
  dsp.display_hertz (formattedHertz);
}

void service_band (uint8_t band) {
  if (band != PreviousBand) {
    PreviousBand = band;
    String formattedBand = rig.band_to_name (band);
    dsp.display_band (formattedBand);
  } 
}

void service_antenna (antennas antenna) {
  if (antenna !=  PreviousAntenna) {
    PreviousAntenna = antenna;
    String formattedAntenna = rig.antenna_to_name (antenna);
    dsp.display_antenna (formattedAntenna);
    dsp.display_atu (antenna);
    digitalWrite (RELAY_HEXBEAM_PIN, antenna == HEXBEAM);
    digitalWrite (RELAY_APERIODIC_PIN, antenna == APERIODIC);
  }
}

//----------------------------------------------------------------------- RADIO LOOP MTHODS

bool elapsed_radio_read_millis () {
  uint16_t frequencyReadDelay = 200;  //We can modify for change the lapse of polling

  if (InitFrequencyReadDelay + frequencyReadDelay > millis ())
    return false;
    
  InitFrequencyReadDelay = millis ();
  return true;
}

void on_radio_state_change (rigstate rigState) {
  PreviousRigState = rigState;
  dsp.clear_radio_box ();
  restore_initial_radio_parameters ();

  if (rigState == OFF) {
    dsp.display_radio_off ();
    digitalWrite (RELAY_HEXBEAM_PIN, LOW);
    digitalWrite (RELAY_APERIODIC_PIN, LOW); 
    return;   
  }
  
  dsp.display_antenna_mode (AUTOMATIC);
}

void on_up_button_pressed () {
  antennas activeAntenna = PreviousAntenna == HEXBEAM ? APERIODIC : HEXBEAM;
  service_antenna(activeAntenna);
}

void on_automatic_mode (bands activeBand) {
  if (activeBand == b0)
    return;
    
  antennas activeAntenna = rig.decode_antenna(activeBand);
  service_antenna(activeAntenna);
}
  
void on_read_frequency () {
  uint32_t frequency = rig.decode_frequency ();
    
  #ifdef DEBUG_FREQUENCY_READ
    Serial.print ("Freccuencia = ");
    Serial.println (frequency);
  #endif//DEBUG_FREQUENCY_READ
    
  if (frequency <= 181000  || frequency >= 5200000)
    return; 

  service_frequency (frequency);
  bands activeBand = rig.decode_band (frequency);

  #ifdef DEBUG_ACTIVE_BAND
    Serial.print ("Banda Activa = ");
    Serial.println (rig.band_to_name (activeBand));
  #endif  //DEBUG_ACTIVE_BAND
    
  service_band (activeBand);

  if (AntennaMode == AUTOMATIC)
    on_automatic_mode (activeBand);
}

//-------------------------------------------------------------- SETTING RADIO

void init_rig () {
  rigstate rigStatus = rig.read_power_state();

  if (rigStatus == OFF) {
    dsp.display_radio_off ();
    return;
  } 
     
  RadioOn = true;
  dsp.display_antenna_mode (AUTOMATIC);
}

//---------------------------------------------------------- WATCH

bool elapsed_datatime_read_millis () {
  uint16_t datetimeReadDelay = 1000;  

  if (InitDatetimeReadDelay + datetimeReadDelay > millis ())
    return false;
    
  InitDatetimeReadDelay = millis ();
  return true;  
}

//------------------------------------------------------------ SETUP

void setup () {
  const uint32_t serial0Rate = 115200;
  const uint16_t serial1Rate = 19200;
    
  Serial.begin (serial0Rate);
  Serial.flush ();
  Serial1.begin (serial1Rate);
  Serial1.flush ();

  wtc.init ();

  dsp.init_display ();
  rig.init_relays ();

  init_rig ();
  btn.init_buttons ();
  wtc.init_watch ();
}

//---------------------------------------------------------------- LOOP

void loop () {  
  bool frequencyRead;

  rigstate rigStatus = rig.read_power_state();
  if (PreviousRigState != rigStatus)
    on_radio_state_change (rigStatus);

  if (rigStatus == ON) {
    if (btn.button_pressed (ANTENNA_MODE_BUTTON_PIN)) {
      AntennaMode = AntennaMode == AUTOMATIC ? MANUAL : AUTOMATIC;
      dsp.display_antenna_mode (AntennaMode);
    }

    if (elapsed_radio_read_millis ())
      rig.request_frequency ();

    if (Serial1.available ())
      frequencyRead = rig.read_frequency ();

    #ifdef DEBUG_FRAME
      rig.debug_frame ();
    #endif  //DEBUG_FRAME

    if (frequencyRead) 
      on_read_frequency ();

    if (AntennaMode == MANUAL && btn.button_pressed(UP_BUTTON_PIN))
      on_up_button_pressed();   
  }

  if (btn.button_pressed (DATETIME_MODE_BUTTON_PIN))
    DatetimeMode = DatetimeMode == NORMAL ? SETTING : NORMAL;

  if (DatetimeMode == SETTING) {
    wtc.set_datetime_service ();
    wtc.init_watch ();
    DatetimeMode = NORMAL;  
  }
    

  if (DatetimeMode == NORMAL && elapsed_datatime_read_millis ())
    wtc.service_datetime ();
}
