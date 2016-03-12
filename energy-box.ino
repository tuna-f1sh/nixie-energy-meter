#include <ESP8266wifi.h>
#include <NixieTube.h>
#include <SoftwareSerial.h>

#include "energy-box.h"

// Server port
#define SERVER_PORT "2121"
// Delay between prints and rolling filter calc
#define PRINT_DELAY 2000
// Uncomment to set these if not using private header
#define SSID "energy-box"
#define PASSWORD "energy12"

// Software serial for ESP8266
SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// Wifi
ESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin, Serial);//adding Serial enabled local echo and wifi debug

// NixieTube(DIN,ST,SH,OE,NUM)
NixieTube tube(11, 12, 13, 10, 2);

// Print power to Nixie display
// Sets colon and SI unit
void printPwr(uint16_t Pwr);
// Process commands from TCP
void processCommand(WifiMessage msg);

uint8_t wifi_started = false;
uint32_t print_delay = false;

void setup()
{  
  // create serial port
  swSerial.begin(115200);
  Serial.begin(115200);
  while(!Serial) {;;}
  Serial.println("Booting..");

  // set SI pins as output
  SI_DDR |= SI_PIN_MASK;
  // set SI pins off (high)
  SI_PORT |= SI_OFF_MASK;

  tube.setBrightness(0xff);
  tube.setBackgroundColor((Color) 2);
  tube.setNumber(0);
  tube.setColon((Colon) Both);
  tube.display();

  // --- Fancy boot animation ---
  for (int i = 0; i < 10; i++) {
    tube.setNumber(i);
    tube.display();
    SI_PORT &= ~(SI_PIN_MASK & (1 << ( SI_HOUR + i )));
    delay(100);
  }

  for (int i = 9; i >= 0; i--) {
    tube.setNumber(i);
    tube.display();
    SI_PORT |= (SI_PIN_MASK & (1 << ( SI_HOUR + i )));
    delay(100);
  }

  SI_PORT &= SI_WATT_MASK;
  // --- -------------------- ---

  Serial.println("Starting wifi");
  wifi.setTransportToTCP();// this is also default
  wifi.endSendWithNewline(false); // Will end all transmissions with a newline and carrage return ie println.. default is true
  wifi_started = wifi.begin();
  if (wifi_started) {
    wifi.connectToAP(SSID, PASSWORD);
    /* wifi.startLocalServer(SERVER_PORT);*/
    wifi.connectToServer("192.168.4.1","2121");

    tube.setBackgroundColor((Color) White);
    tube.display();
  } else {
    tube.setBackgroundColor((Color) Red);
    tube.display();
  }

  Serial.println("JBR Energy Monitor Up");
}

void loop()
{
  uint16_t Pwr = 0;
  char espBuf[1];

  //Make sure the esp8266 is started..
  if (!wifi.isStarted())
    wifi.begin();

  espBuf[0] = 0xC0;
  wifi.send(SERVER, espBuf, false);

  print_delay = millis();
  WifiMessage in = wifi.listenForIncomingMessage(2000);
  if (in.hasData) {
    Pwr = (uint16_t) ((in.message[1] & 0x7F) + ((in.message[2] << 7) & 0x7F));
    swSerial.println(Pwr);
  }

  // display power
  printPwr(Pwr);

  // wait for min time
  while (millis() - print_delay < PRINT_DELAY ) {
    ;;
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
  Serial.print(str);
  Serial.println(set);
}
