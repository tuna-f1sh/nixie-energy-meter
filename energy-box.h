// ESP8266 PINS
#define sw_serial_rx_pin 4 //  Connect this pin to TX on the esp8266
#define sw_serial_tx_pin 6 //  Connect this pin to RX on the esp8266
#define esp8266_reset_pin 5 // Connect this pin to CH_PD on the esp8266, not reset. (let reset be unconnected)

#define EEPROM_FLAG 0
#define EEPROM_FLAG_VALUE 0xee
#define EEPROM_CAL 1

#define SI_PORT PORTC
#define SI_DDR  DDRC
#define SI_HOUR PC2
#define SI_WATT PC3
#define SI_KILO PC4
#define SI_MEGA PC5

#define SI_PIN_MASK   (1 << SI_MEGA | 1 << SI_KILO | 1 << SI_WATT | 1 << SI_HOUR)
#define SI_OFF_MASK   (1 << SI_MEGA | 1 << SI_KILO | 1 << SI_WATT | 1 << SI_HOUR)
#define SI_WATT_MASK  (1 << SI_MEGA | 1 << SI_KILO | 1 << SI_HOUR)
#define SI_HOUR_MASK  (1 << SI_MEGA | 1 << SI_KILO | 1 << SI_WATT)
#define SI_KILO_MASK  (1 << SI_MEGA | 1 << SI_WATT | 1 << SI_HOUR)
#define SI_MEGA_MASK  (1 << SI_KILO | 1 << SI_WATT | 1 << SI_HOUR)
