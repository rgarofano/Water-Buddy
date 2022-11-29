SRC_C_main = main.c i2c.c

PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C   = $(CROSS_TOOL)gcc

C_FLAGS = -Wall -Wshadow -std=c99 -D_POSIX_C_SOURCE=200809L -g

main:
	$(CC_C) $(C_FLAGS) $(SRC_C_main) -o $(OUTDIR)/i2cled
clean:
	rm -f *$(OUTDIR)/o
