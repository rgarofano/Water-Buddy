#include "i2c.h"

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100
#define Rw 0b00000010
#define Rs 0b00000001 

int initLCD(){
    int initBus = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2c(initBus, LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE, 0);
    writeI2c(initBus, LCD_DISPLAYCONTROL | LCD_DISPLAYON, 0);
    writeI2c(initBus, LCD_CLEARDISPLAY, 0);
    writeI2c(initBus, LCD_ENTRYMODESET | LCD_ENTRYLEFT, 0);
    printf("LCD has been initialized\n");
    return initBus;
}

void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it 
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        break;
        // printf("--> %s", buffer); // Uncomment for debugging
        }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

void configureI2C() {
	runCommand("config-pin p9.19 i2c");
	runCommand("config-pin p9.20 i2c");
}

int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("I2C: Unable to set I2C device to slave address.");
		exit(1);
	}
	return i2cFileDesc;
}

void writeCMD(int i2cFileDesc, unsigned char regAddr)
{
	unsigned char buffer[1];
	buffer[0] = regAddr;
	int res = write(i2cFileDesc, buffer, 1);
	if (res != 1) {
		perror("I2C: Unable to write i2c register.");
		exit(1);
	}
}

void writeI2cFour(int i2cFileDesc, unsigned char cmd){
	writeCMD(i2cFileDesc, cmd | LCD_BACKLIGHT);
	lcdStrobe(i2cFileDesc, cmd);
}

void writeI2c(int i2cFileDesc, unsigned char cmd, int mode)
{
	writeI2cFour(i2cFileDesc, mode | (cmd & 0xF0));
	writeI2cFour(i2cFileDesc, mode | ((cmd << 4) & 0xF0));
}

void lcdStrobe(int i2cFileDesc, unsigned char cmd){
	writeCMD(i2cFileDesc, (cmd | En | LCD_BACKLIGHT));
	sleepForMs(10);
	writeCMD(i2cFileDesc, ((cmd & ~En) | LCD_BACKLIGHT));
	sleepForMs(10);
}

void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

long long getTimeInMs(void) 
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long nanoSeconds = spec.tv_nsec;
    return nanoSeconds;
}

void lcd_display_string(int i2cFileDesc, char* string, int line){
    if (line == 1)
        writeI2c(i2cFileDesc, 0x80, 0);
    if (line == 2)
        writeI2c(i2cFileDesc, 0xC0, 0);
    if (line == 3)
        writeI2c(i2cFileDesc, 0x94, 0);
    if (line == 4)
        writeI2c(i2cFileDesc, 0xD4, 0);

    int i=0;
    char next = string[i];
    while(next){
        i++;
        writeI2c(i2cFileDesc, (int) next, 1);
        next = string[i];
    }
}