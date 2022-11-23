
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread -Werror
# -Werror: treat warnings as errors

HW_COMMON_FILES = ../hwCommon/SystemTools.c ../hwCommon/SPI.c ../hwCommon/I2C.c ../hwCommon/GPIO.c ../hwCommon/A2D.c

all: clean spidev_test RFIDReader_test

# Test Targets
spidev_test: clean_spidev_test
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

RFIDReader_test: clean_RFIDReader_test
	$(CC) $(CFLAGS) RFIDReader_test.c ../RFIDReader.c $(HW_COMMON_FILES) -o RFIDReader_test
	cp RFIDReader_test $(HOME)/cmpt433/public/myApps/

# Clean Targets
clean_spidev_test:
	rm -f spidev_test

clean_RFIDReader_test:
	rm -f RFIDReader_test

clean:
	rm -f spidev_test RFIDReader_test
