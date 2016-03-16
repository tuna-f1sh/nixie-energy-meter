#include <EmonLib.h>
#include <NixieTube.h>
#include <EEPROM.h>

#include "energy-box.h"

// Delay between prints and rolling filter calc
#define PRINT_DELAY 2000
// Uncomment to set these if not using private header
/* #define SSID "yourssid"*/
/* #define PASSWORD "yourpassword"*/

// SSID in header to keep private from REPO
#ifndef SSID
#include "ssid.h"
#endif

// Create current transformer object
EnergyMonitor emon;

// NixieTube(DIN,ST,SH,OE,NUM)
NixieTube tube(11, 12, 13, 10, 2);

// Print power to Nixie display
// Sets colon and SI unit
void printPwr(uint16_t Pwr);
// Update rolling averages
inline void updateRoll(uint8_t index);

// Rolling averages calc
uint32_t print_delay = 0;
uint32_t roll[4] = {0};
uint32_t avg[4] = {0};
uint32_t count[4] = {0};

rolling_t display = second;

void setup()
{  
  uint8_t adc_cal = 0;
  uint8_t flag = 0;

  // create serial port
  Serial.begin(9600);
  while(!Serial) {;;}

  // set SI pins as output
  SI_DDR |= SI_PIN_MASK;
  // set SI pins off (high)
  SI_PORT |= SI_OFF_MASK;

  // emon current calibrate with EEPROM if exist
  flag = EEPROM.read(EEPROM_FLAG);
  if (flag == EEPROM_FLAG_VALUE) {
    adc_cal = EEPROM.read(EEPROM_CAL);
    emon.current(0, adc_cal); // cal saved in EEPROM
  // EEPROM not programmed, set flag and save default calibration
  } else {
    emon.current(0, 29); // calibrated with meter
    EEPROM.write(EEPROM_FLAG,EEPROM_FLAG_VALUE);
    EEPROM.write(EEPROM_CAL,29);
    /* emon.current(0, 60.6); // 33ohm*/
    /* emon.current(0, 111.1); // 18ohm*/
  }

  tube.setBrightness(0xff);
  tube.setBackgroundColor((Color) 2);
  tube.setNumber(0);
  tube.setColon((Colon) Both);
  tube.display();

  for (int i = 0; i < 10; i++) {
    tube.setNumber(i);
    tube.display();
    SI_PORT &= ~(SI_PIN_MASK & (1 << SI_HOUR + i));
    delay(100);
  }

  for (int i = 9; i >= 0; i--) {
    tube.setNumber(i);
    tube.display();
    SI_PORT |= (SI_PIN_MASK & (1 << SI_HOUR + i));
    delay(100);
  }

  SI_PORT &= SI_WATT_MASK;

  tube.setBackgroundColor((Color) White);
  tube.display();
  Serial.println("JBR Energy Monitor Up");
}

void loop()
{
  double Irms = 0.0;
  uint16_t Pwr = 0;

  Irms = emon.calcIrms(1480);
  Pwr = Irms * 230;

  // Update rolling averages
  if (millis() - print_delay > PRINT_DELAY) {
    updateRoll(0);
    printPwr(avg[display]);
    print_delay = millis();
    /* swSerial.print(roll[0]);*/
    /* swSerial.println(roll[1]);*/
  } else {
    count[0]++;
    roll[0] += Pwr;
  }

  if (count[1] == (60 / (PRINT_DELAY / 1000))) {
    updateRoll(1);
  }

  if (count[2] == 60) {
    updateRoll(2);
  }

  if (count[3] == 24) {
    updateRoll(3);
  }
}

void printPwr(uint16_t Pwr) {
  tube.printf("%02d",Pwr);
  SI_PORT |= (SI_OFF_MASK & SI_WATT_MASK);
  
  if ((Pwr >= 100) && (Pwr < 1000)) {
    tube.setColon(1,(Colon) Lower);
  } else if ((Pwr >= 1000) && (Pwr < 10000)) {
    SI_PORT &= SI_KILO_MASK;
    tube.setColon(0,(Colon) Upper);
  } else if (Pwr >= 10000) {
    SI_PORT &= SI_MEGA_MASK;
    tube.setColon(1,(Colon) Upper);
  } else {
    tube.setColon(1,(Colon) None);
  }

  // set kilowatt hours
  if (display == (rolling_t) hour)
    SI_PORT &= SI_HOUR_MASK;
  
  tube.display();
}

inline void updateRoll(uint8_t index) {
  // work out average
  roll[index] /= count[index];
  // set new average
  avg[index] = roll[index];
  // clear rolling accumulators
  roll[index] = 0;
  count[index] = 0;
  if ( index < (sizeof(roll) / sizeof(roll[0])) ) {
    // inc next time roll
    roll[index+1] += avg[index];
    count[index+1]++;
  }
}
