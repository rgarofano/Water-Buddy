
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread #-Werror
# -Werror: treat warnings as errors

HW_COMMON = $(wildcard ../hwCommon/*.c)
DEVICES = $(wildcard ../devices/*.c)

all: clean spidev_test RFIDReader_test scale_test1

# Test Targets
spidev_test: clean
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

RFIDReader_test: clean
	$(CC) $(CFLAGS) RFIDReader_test.c ../devices/RFIDReader.c $(HW_COMMON) -o RFIDReader_test
	cp RFIDReader_test $(HOME)/cmpt433/public/myApps/

scale_test1: clean
	$(CC) $(CFLAGS) scale_test1.c ../devices/HX711_ADC.c $(HW_COMMON) -o scale_test1
	cp scale_test1 $(HOME)/cmpt433/public/myApps/

# Clean Target
clean:
	rm -f spidev_test RFIDReader_test scale_test1
