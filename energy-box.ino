#include <SPI.h>
#include <RF24.h>
#include <NixiePipe.h>

#include "energy-box.h"

// Delay between prints and rolling filter calc
#define PRINT_DELAY 2000

NixiePipe npipes = NixiePipe(3,2);

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// Print power to Nixie display
// Sets colon and SI unit
void printPwr(uint16_t Pwr);

uint8_t wifi_started = false;
uint32_t print_delay = false;
CRGB gMainRGB = CRGB::OrangeRed;

void setup()
{  
  // create serial port
  Serial.begin(115200);
  while(!Serial) {;;}
  Serial.println("Booting..");

  npipes.begin();
  npipes.clear();
  npipes.setBrightness(255);
  npipes.setPipeColour(gMainRGB);
  npipes.setPipeNumber(0, Unit::Watts);
  npipes.setPipeNumber(1, Prefix::Mega);
  npipes.show();

  // --- Fancy boot animation ---
  for (int i = 0; i < 10; i++) {
    npipes.writeNumber(i);
    npipes.show();
    delay(100);
  }

  for (int i = 9; i >= 0; i--) {
    npipes.writeNumber(i);
    npipes.show();
    delay(100);
  }

  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.printDetails();

  radio.startListening();
  radio.printDetails();

  Serial.println("JBR Energy Monitor Up");
}

void loop()
{
  static uint16_t Pwr = 0;
  uint8_t req = 0xC0;
  bool timeout = false;
  static uint8_t count = 0;
  
  // stop listening to talk
  radio.stopListening();

  // send power request
  if (radio.write( &req, sizeof(req) ))
    Serial.println("Sent request");
  else
    Serial.println("Request failed");

  // resume listening
  radio.startListening();

  while( !radio.available() && !timeout) {
    timeout = (millis() - print_delay < PRINT_DELAY) ? false : true;
  }

  if (!timeout) {
    radio.read( &Pwr, sizeof(Pwr) );
    npipes.setPipeColour(gMainRGB);
    Serial.print("Pwr: ");
    Serial.println(Pwr);
    count = 0;
  } else {
    if (count > 5) {
      npipes.setPipeColour(CRGB::Red);
      Pwr = 0;
    }
    Serial.println("No reply..");
    timeout = false;
    count++;
  }

  // wait for min time
  while (millis() - print_delay < PRINT_DELAY );

  // display power
  printPwr(Pwr);
  print_delay = millis();
}

void printPwr(uint16_t Pwr) {
  npipes.writeNumber( (Pwr % 1000) );
  
  if (Pwr < 1000) {
    npipes.setPipeColour(1, CRGB::Black);
  } else if ((Pwr >= 1000) && (Pwr < 10000)) {
    npipes.setPipeNumber(1, Prefix::Kila);
  } else if (Pwr >= 10000) {
    npipes.setPipeNumber(1, Prefix::Mega);
  }
  
  npipes.write();
  npipes.show();
}
