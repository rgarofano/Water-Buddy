
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L# -pthread -Werror
# -Werror: treat warnings as errors

FILE_LIST = main.c SystemTools.c GPIO.c A2D.c I2C.c SPI.c RFIDReader.c
OUTPUT = main

all: clean $(OUTPUT)

# Main Target
$(OUTPUT): clean
	$(CC) $(CFLAGS) $(FILE_LIST) -o $(OUTPUT)
	cp $(OUTPUT) $(HOME)/cmpt433/public/myApps/

# Test Targets
spidev_test: cleanspitest
	$(CC) $(CFLAGS) spidev_test.c -o spidev_test
	cp spidev_test $(HOME)/cmpt433/public/myApps/

rfid_test: cleanrfidtest
	$(CC) $(CFLAGS) RFIDReader_test.c SystemTools.c GPIO.c SPI.c RFIDReader.c -o rfid_test
	cp rfid_test $(HOME)/cmpt433/public/myApps/

# Clean Targets
clean:
	rm -f $(OUTPUT)

cleanspitest:
	rm -f spidev_test

cleanrfidtest:
	rm -f rfid_test

cleantests:
	rm -f spidev_test rfid_test

# Clean main and all tests
cleanall: clean cleantests