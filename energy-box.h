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

enum rolling_t
{
  second,
  minute,
  hour,
  day
};
