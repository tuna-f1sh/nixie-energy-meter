#include <SoftwareSerial.h>
#include <ESP8266wifi.h>
#include <EmonLib.h>
#include "commands.h"

#define sw_serial_rx_pin 4 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 6 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 5 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define PRINT_DELAY 2000

// Create current transformer object
EnergyMonitor emon;

SoftwareSerial swSerial(sw_serial_rx_pin, sw_serial_tx_pin);

// the last parameter sets the local echo option for the ESP8266 module..
ESP8266wifi wifi(swSerial, swSerial, esp8266_reset_pin, Serial);//adding Serial enabled local echo and wifi debug

void processCommand(WifiMessage msg);
void sendValueAsTwo7bitBytes(uint8_t channel, uint16_t value);

uint8_t wifi_started = false;
uint16_t Pwr = 0;
uint32_t print_delay = 0;
uint32_t roll = 0;
uint32_t count = 0;
uint16_t avg_power = 0;

void setup() {
  swSerial.begin(9600);
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Starting wifi");

  emon.current(0, 29); // calibrated with meter

  wifi.setTransportToTCP();// this is also default
  // wifi.setTransportToUDP();//Will use UDP when connecting to server, default is TCP

  wifi.endSendWithNewline(false); // Will end all transmissions with a newline and carrage return ie println.. default is true

  wifi_started = wifi.begin();

  //Turn on local ap and server (TCP)
  wifi.startLocalAPAndServer("energy-box", "energy12", "5", "2121");
}

void loop() {
  static WifiConnection *connections;
  double Irms = 0.0;

  Irms = emon.calcIrms(1480);
  Pwr = Irms * 230;

  //Make sure the esp8266 is started..
  if (!wifi.isStarted())
    wifi.begin();

  // check connections if the ESP8266 is there
  if (wifi_started)
    wifi.checkConnections(&connections);

  // Update rolling averages
  if (millis() - print_delay > PRINT_DELAY) {
    avg_power = roll / count;
    roll = 0;
    count = 0;
    print_delay = millis();
  } else {
    count++;
    roll += Pwr;
  }

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

  /* WifiMessage in = wifi.listenForIncomingMessage(6000);*/
  /* if (in.hasData) {*/
  /*   if (in.channel == SERVER)*/
  /*     Serial.println("Message from the server:");*/
  /*   else*/
  /*     Serial.println("Message a local client:");*/
  /*   Serial.println(in.message);*/
  /*   //Echo back;*/
  /*   wifi.send(in.channel, "Echo:", false);*/
  /*   wifi.send(in.channel, in.message);*/
  /*   nextPing = millis() + 10000;*/
  /* }*/
}

void processCommand(WifiMessage msg) {
  // return buffer
  char espBuf[1];
  // scanf holders
  int set;
  char str[16];
  uint8_t i =  0;

  // Get command and setting
  Serial.println(msg.message);
  sscanf(msg.message,"%15s %d",str,&set);

  // Report power replies with power
  switch ((uint8_t) msg.message[i++]) {
    case (REPORT_POWER):
      espBuf[0] = REPORT_POWER;
      // send command byte
      wifi.send(msg.channel,espBuf,false);
      // followed by data
      /* sendValueAsTwo7bitBytes(msg.channel,255);*/
      Serial.println(Pwr);
      espBuf[0] = avg_power & 0x7F;
      wifi.send(msg.channel,espBuf,false);
      espBuf[0] = avg_power >> 7 & 0x7F;
      wifi.send(msg.channel,espBuf,true);
      /* espBuf[0] = REPORT_POWER;*/
      /* wifi.send(msg.channel,espBuf,true);*/
      break;
    default:
      Serial.println("Unknown command");
      break;
  }
}

void sendValueAsTwo7bitBytes(uint8_t channel, uint16_t value)
{
  char buf[1];
  buf[0] = 128; // MSB
  wifi.send(channel,buf,false);
  buf[0] = 128; // LSB
  /* buf[0] = 128;*/
  /* buf[1] = 128;*/
  wifi.send(channel,buf,false);
}
