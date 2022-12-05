
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread #-Werror
# -Werror: treat warnings as errors

HW_COMMON = $(wildcard ../hwCommon/*.c)
DEVICES = $(wildcard ../devices/*.c)

all: clean

# Test Targets
spidev_test: clean
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

RFIDReader_test: clean
	$(CC) $(CFLAGS) RFIDReader_test.c ../devices/RFIDReader.c $(HW_COMMON) -o RFIDReader_test
	cp RFIDReader_test $(HOME)/cmpt433/public/myApps/

Scale_test: clean
	$(CC) $(CFLAGS) Scale_test.c ../devices/Scale.c $(HW_COMMON) -o Scale_test
	cp Scale_test $(HOME)/cmpt433/public/myApps/

LCD_test: clean
	$(CC) $(CFLAGS) LCD_test.c ../devices/LCDDisplay.c $(HW_COMMON) -o LCD_test
	cp LCD_test $(HOME)/cmpt433/public/myApps/

clean:
	rm -f spidev_test RFIDReader_test LCD_test Scale_test
