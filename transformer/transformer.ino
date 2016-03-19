#include <SPI.h>
#include <EmonLib.h>
#include <RF24.h>

// Create current transformer object
EnergyMonitor emon;

RF24 radio(8,9);

#define PRINT_DELAY 2000

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

uint16_t Pwr = 0;
uint32_t print_delay = 0;
uint32_t roll = 0;
uint32_t count = 0;
uint16_t avg_power = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("Booting..");

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  emon.current(0, 29); // calibrated with meter

  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);

  radio.startListening();

  Serial.println("Transformer Up");
}

void loop() {
  float Irms = 0.0;
  uint8_t command;
  static uint8_t status = false;
  static uint32_t timeout;

  Irms = emon.calcIrms(1480);
  Pwr = Irms * 230;

  // check connections if the ESP8266 is there
  if (radio.available()) {
    status = radio.read( &command, sizeof(command) );
    Serial.print("Received: ");
    Serial.println(command);
    timeout = millis();
    if (command == 0xC0) {
      delay(20);
      radio.stopListening();
      status = radio.write(&Pwr, sizeof(Pwr));
      Serial.print("Sent: ");
      Serial.println(Pwr);
      radio.startListening();
    }
  }

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

  // Update status LED
  if (status && (millis() - timeout < PRINT_DELAY)) {
    digitalWrite(2,HIGH); // green
    digitalWrite(3,LOW);
  } else {
    digitalWrite(2,LOW); // orange
    digitalWrite(3,HIGH);
  }
}
