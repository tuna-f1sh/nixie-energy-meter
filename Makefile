BOARD_TAG    = uno
# mini settings
#BOARD_TAG    = nano
#MCU = atmega328p
#F_CPU = 16000000
#AVRDUDE_ARD_BAUDRATE = 57600
# # FTDI
#MONITOR_PORT = /dev/tty.SLAB*
# MONITOR_PORT = /dev/tty.wchusb*

ARDUINO_LIBS = FastLED RF24 NixiePipe SPI
AVRDUDE_OPTS = -V

# compiler flags for FastLED lib
CXXFLAGS_STD = -std=gnu++11
CXXFLAGS += -fno-threadsafe-statics

# ARDUINO_QUIET = 1

include ${ARDMK_DIR}/Arduino.mk
