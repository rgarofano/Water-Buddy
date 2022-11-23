
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread -Werror
# -Werror: treat warnings as errors

HW_COMMON_FILES = hwCommon/SystemTools.c hwCommon/SPI.c hwCommon/I2C.c hwCommon/GPIO.c hwCommon/A2D.c
SOURCES = main.c RFIDReader.c $(HW_COMMON_FILES)
TARGET = main

all: clean $(TARGET)

# Main Target
$(TARGET): clean
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	cp $(TARGET) $(HOME)/cmpt433/public/myApps/

# Clean Target
clean:
	rm -f $(TARGET)