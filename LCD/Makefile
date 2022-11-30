SRC_C_main = main.c i2c.c lcd.c General.c

SRC_C_PCF = pcf8574-lcd-demo.c

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C   = $(CROSS_TOOL)gcc

C_FLAGS = -Wshadow -Wall -std=c99 -D_POSIX_C_SOURCE=200908L -g

main:
	$(CC_C) $(C_FLAGS) $(SRC_C_main) -o $(OUTDIR)/i2ctest
PCF:
	$(CC_C) $(C_FLAGS) $(SRC_C_PCF) -o $(OUTDIR)/i2cledPCF
clean:
	rm -f *$(OUTDIR)/o
