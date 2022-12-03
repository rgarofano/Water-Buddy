
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread -Werror
# -Werror: treat warnings as errors

HW_COMMON = $(wildcard ../hwCommon/*.c)
DEVICES = $(wildcard ../devices/*.c)

all: clean spidev_test RFIDReader_test

# Test Targets
spidev_test: clean_spidev_test
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

RFIDReader_test: clean_RFIDReader_test
	$(CC) $(CFLAGS) RFIDReader_test.c ../devices/RFIDReader.c $(HW_COMMON) -o RFIDReader_test
	cp RFIDReader_test $(HOME)/cmpt433/public/myApps/

LCD_test: clean_LCD_test
	$(CC) $(CFLAGS) LCD_test.c ../devices/LCDDisplay.c $(HW_COMMON) -o LCD_test
	cp LCD_test $(HOME)/cmpt433/public/myApps/

# Clean Targets
clean_spidev_test:
	rm -f spidev_test

clean_RFIDReader_test:
	rm -f RFIDReader_test

clean_LCD_test:
	rm -f LCD_test

clean:
	rm -f spidev_test RFIDReader_test LCD_test
