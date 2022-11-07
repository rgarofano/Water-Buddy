
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L# -pthread -Werror
# -Werror: treat warnings as errors

FILE_LIST = main.c SystemTools.c HardwareIO.c RFID.c
OUTPUT = main

all: clean $(OUTPUT)

$(OUTPUT): clean
	$(CC) $(CFLAGS) $(FILE_LIST) -o $(OUTPUT)
	cp $(OUTPUT) $(HOME)/cmpt433/public/myApps/

spidev_test: cleanspitest
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

clean:
	rm -f $(OUTPUT)

cleanspitest:
	rm -f spidev_test

cleanall: clean cleanspitest