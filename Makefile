
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -pthread -Werror
# -Werror: treat warnings as errors

HW_COMMON = $(wildcard hwCommon/*.c)
DEVICES = $(wildcard devices/*.c)
SW_MODULES = $(wildcard swModules/*.c)
SOURCES = main.c $(HW_COMMON) $(DEVICES) $(SW_MODULES)
TARGET = WaterBuddy

PUBDIR = $(HOME)/cmpt433/public/myApps

all: $(TARGET)

# Main Target
$(TARGET): clean
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	cp $(TARGET) $(PUBDIR)/
	mkdir -p $(PUBDIR)/server/
	cp -R server/* $(PUBDIR)/server/
	cp formData.json $(PUBDIR)/
	cp server/.env $(PUBDIR)/server/
	

# Clean Target
clean:
	rm -f $(TARGET)