
BOARD_TAG    = uno
ARDUINO_LIBS = DualVNH5019MotorShield motor_controll main PinChangeInt

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
ARDUINO_PORT = /dev/ttyACM0
include /home/Development/Arduino/Arduino-Makefile/arduino-mk/Arduino.mk
endif

ifeq ($(UNAME), Darwin)
ARDUINO_DIR = /Applications/Arduino.app/Contents/Resources/Java
PARSE_BOARD = ../../../arduino-mk/ard-parse-boards
ARDUINO_PORT = /dev/cu.usbmodemfa131
include ../../arduino-mk/Arduino.mk
endif

