BOARD_TAG    = uno
# mini settings
#BOARD_TAG    = nano
#MCU = atmega328p
#F_CPU = 16000000
#AVRDUDE_ARD_BAUDRATE = 57600
# # FTDI
# MONITOR_PORT = /dev/tty.SLAB*
#MONITOR_PORT = /dev/tty.wchusb*

ARDUINO_LIBS = EmonLib SoftwareSerial ESP8266wifi NixieTubev2 EEPROM
AVRDUDE_OPTS = -V

# ARDUINO_QUIET = 1

include ${ARDMK_DIR}/Arduino.mk
