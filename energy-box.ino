#include <SPI.h>
#include <RF24.h>
#include <NixieTube.h>

#include "energy-box.h"

// Delay between prints and rolling filter calc
#define PRINT_DELAY 2000

// NixieTube(DIN,ST,SH,OE,NUM)
NixieTube tube(2, 3, 4, 5, 2);

RF24 radio(8,9);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// Print power to Nixie display
// Sets colon and SI unit
void printPwr(uint16_t Pwr);

uint8_t wifi_started = false;
uint32_t print_delay = false;

void setup()
{  
  // create serial port
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

  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  radio.startListening();
  radio.printDetails();

  Serial.println("JBR Energy Monitor Up");
  tube.setBackgroundColor((Color) White);
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
    tube.setBackgroundColor((Color) White);
    Serial.print("Pwr: ");
    Serial.println(Pwr);
    count = 0;
  } else {
    if (count > 5) {
      tube.setBackgroundColor((Color) Red);
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
