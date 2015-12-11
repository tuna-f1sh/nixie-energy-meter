#include <EmonLib.h>
#include <ESP8266wifi.h>
#include <NixieTube.h>
#include <SoftwareSerial.h>

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

// Software serial for ESP8266
SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// Wifi
ESP8266wifi wifi(Serial, Serial, esp8266_reset_pin/*, swSerial*/);//adding Serial enabled local echo and wifi debug

// NixieTube(DIN,ST,SH,OE,NUM)
NixieTube tube(11, 12, 13, 10, 2);

// Print power to Nixie display
// Sets colon and SI unit
void printPwr(uint16_t Pwr);
// Process commands from TCP
void processCommand(WifiMessage msg);
inline void updateRoll(uint8_t index);

// TCP Commands
const char STRM[] PROGMEM = "STRM";
const char ROLL[] PROGMEM = "ROLL";
const char COLOUR[] PROGMEM = "COLOUR";
const char DISP[] PROGMEM = "DISP";
const char RESET[] PROGMEM = "RESET";
const char RST[] PROGMEM = "RST";
const char IDN[] PROGMEM = "*IDN?";

// JSON template
const char* json = "{\"sec\":\"%lu\",\"min\":\"%lu\",\"hour\":\"%lu\",\"day\":\"%lu\"}";

// Rolling averages calc
uint32_t print_delay = 0;
uint32_t roll[4] = {0};
uint32_t avg[4] = {0};
uint32_t count[4] = {0};

rolling_t display = second;

void setup()
{  
  // create serial port
  /* swSerial.begin(9600);*/
  Serial.begin(9600);
  while(!Serial) {;;}
  /* swSerial.println("Booting..");*/

  // emon current calibrate and ADC pin
  emon.current(0, 29); // calibrated with meter
  /* emon.current(0, 60.6); // 33ohm*/
  /* emon.current(0, 111.1); // 18ohm*/

  tube.setBrightness(0xff);
  tube.setBackgroundColor((Color) Blue);
  tube.setNumber(0);
  tube.setColon((Colon) Both);
  tube.display();

  /* swSerial.println("Starting wifi");*/
  wifi.setTransportToTCP();// this is also default
  wifi.endSendWithNewline(false); // Will end all transmissions with a newline and carrage return ie println.. default is true
  wifi.begin();
  wifi.connectToAP(SSID, PASSWORD);
  wifi.startLocalServer("80");
  // TODO get last digit and display value
  /* wifi.getIP();*/

  tube.setBackgroundColor((Color) White);
  tube.display();
  /* swSerial.println("JBR Energy Monitor Up");*/
}

void loop()
{
  static WifiConnection *connections;
  double Irms = 0.0;
  uint16_t Pwr = 0;

  //Make sure the esp8266 is started..
  if (!wifi.isStarted()) {
    wifi.begin();
    wifi.connectToAP(SSID, PASSWORD);
    wifi.startLocalServer("80");
  }

  Irms = emon.calcIrms(1480);
  Pwr = Irms * 230;

  wifi.checkConnections(&connections);
  for (int i = 0; i < MAX_CONNECTIONS; i++) {
    if (connections[i].connected) {
      // See if there is a message
      WifiMessage msg = wifi.getIncomingMessage();
      // Check message is there
      if (msg.hasData) {
        processCommand(msg);
      }
    }
  }

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
  if ((Pwr >= 100) && (Pwr < 1000))
    tube.setColon(1,(Colon) Lower);
  else if ((Pwr >= 1000) && (Pwr < 10000))
    tube.setColon(0,(Colon) Upper);
  else if (Pwr >= 10000)
    tube.setColon(1,(Colon) Upper);
  else
    tube.setColon(1,(Colon) None);
  tube.display();
}

void processCommand(WifiMessage msg) {
  // return buffer
  char espBuf[MSG_BUFFER_MAX];
  // scanf holders
  int set;
  char str[16];

  // Get command and setting
  sscanf(msg.message,"%15s %d",str,&set);
  /* swSerial.print(str);*/
  /* swSerial.println(set);*/

  // Stream JSON
  if ( !strcmp_P(str,STRM) ) {
    snprintf(espBuf,sizeof(espBuf),json,avg[0],avg[1],avg[2],avg[3]);
    wifi.send(msg.channel,espBuf);
  }
  else if ( !strcmp_P(str,ROLL) ) {
    snprintf(espBuf,sizeof(espBuf),json,roll[0],roll[1],roll[2],roll[3]);
    wifi.send(msg.channel,espBuf,false);
    snprintf(espBuf,sizeof(espBuf),json,count[0],count[1],count[2],count[3]);
    wifi.send(msg.channel,espBuf,true);
  }
  // Change colour
  else if ( !strcmp_P(str,COLOUR) ) {
    tube.setBackgroundColor((Color) set);
    tube.display();
    wifi.send(msg.channel,"COLOUR OK");
  }
  // Reset rolling counts
  else if ( !strcmp_P(str,RESET) ) {
    memset(roll, 0, ( (sizeof(roll)/sizeof(roll[0])) * sizeof(roll[0]) ) );
    memset(count, 0, ( (sizeof(count)/sizeof(count[0])) * sizeof(count[0]) ) );
    wifi.send(msg.channel,"COUNT OK");
  }
  // Change Nixie display var
  else if ( !strcmp_P(str,DISP) ) {
    display = (rolling_t) set;
    wifi.send(msg.channel,msg.message);
  }
  // Return what we are
  else if ( !strcmp_P(str,IDN) ) {
    wifi.send(msg.channel,"JBR ENERGY MONITOR V0.1");
  }
  // Reset system by temp enable watchdog
  else if ( !strcmp_P(str,RST) ) {
    wifi.send(msg.channel,"SYSTEM RESET...");
    // soft reset by reseting PC
    asm volatile ("  jmp 0");
  }
  // Unknown command
  else {
    wifi.send(msg.channel,"ERR");
  }
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
