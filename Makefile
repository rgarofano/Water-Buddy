
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread -Werror
# -Werror: treat warnings as errors

HW_COMMON = $(wildcard hwCommon/*.c)
DEVICES = $(wildcard devices/*.c)
SOURCES = main.c $(HW_COMMON) $(DEVICES)
TARGET = WaterBuddy

all: clean $(TARGET)

# Main Target
$(TARGET): clean
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	cp $(TARGET) $(HOME)/cmpt433/public/myApps/

# Clean Target
clean:
	rm -f $(TARGET)