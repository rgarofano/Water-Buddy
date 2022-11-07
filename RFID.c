
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#include "SystemTools.h"
#include "HardwareIO.h"
#include "SPI.h"

#include "RFID.h"

static int      spiBusNum;
static char*    rstPin;
static int      rstGpioNum;
static char*    sdaPin;

static int spiFileDesc = -1;



// Pin Defines
#define RFID_SPI_NUM 1

#define RFID_RST_PIN "p9_11"
#define RFID_RST_GPIO_NUM 30

#define RFID_IRQ_PIN
#define RFID_IRQ_GPIO_NUM

// MISO, Master In Slave Out
#define RFID_MISO_PIN "p9_29" // SPI1_D0

// MOSI, Master Out Slave In
#define RFID_MOSI_PIN "p9_30" // SPI1_D1

// SCK, Clock
#define RFID_SCK_PIN "p9_31" // SPI1_SCLK

// SDA/SS, Slave Select
#define RFID_SDA_PIN "p9_28" // SPI1_CS0

unsigned char singleTransfer(int fd, unsigned char data_byte)
{
	unsigned char rec_byte = 0;

    struct spi_ioc_transfer xfer;
    memset(&xfer, 0, sizeof xfer);

    xfer.bits_per_word = 8;
    xfer.delay_usecs = 0;
    xfer.speed_hz = 50000;

	/* Override No. of bytes to send and receive one byte */
	xfer.len = 1;

	/* Points to the address of Tx and Rx variable  */
	xfer.tx_buf = (unsigned long)&data_byte;
	xfer.rx_buf = (unsigned long)&rec_byte;

	/* Perform an SPI Transaction */
	if (ioctl(fd, SPI_IOC_MESSAGE(1), &xfer) < 0) {
		perror("SPI: SPI_IOC_MESSAGE Failed |");
		rec_byte = -1;
	}

	return (rec_byte);
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
static void transfer(int fd)
{
	int ret;
	uint8_t tx[] = {
		(0x80 | (0x37 << 1)), 0x00
	};

	uint8_t rx[ARRAY_SIZE(tx)] = {0, };

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = 0,
		.speed_hz = 500000,
		.bits_per_word = 8,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		perror("SPI_IOC_MESSAGE");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
}



// From https://www.emcraft.com/stm32f769i-discovery-board/accessing-spi-devices-in-linux
void test1(void)
{
    int fd = spiFileDesc;

    // unsigned char send[16];
    // memset(send, 0, sizeof send);
    // send[0] = 0x80 | (0x37 << 1);
    // unsigned char recv[16];
    // memset(recv, 0, sizeof recv);

    // struct spi_ioc_transfer xfer[2];
    // memset(xfer, 0, sizeof xfer);

    // xfer.bits_per_word      = 8;
    // xfer.cs_change          = 0;
    // xfer.delay_usecs        = 0;
    // xfer.pad                = 0;
    // xfer.rx_nbits           = 0;
    // xfer.tx_nbits           = 0;
    // xfer.word_delay_usecs   = 0;

    // unsigned char response1 = singleTransfer(fd, 0x80 | (0x37 << 1));
    // unsigned char response2 = singleTransfer(fd, 0);

    transfer(fd);

    // xfer[0].tx_buf = (unsigned long)send;
    // xfer[0].len = 1;

    // xfer[1].rx_buf = (unsigned long)recv;
    // xfer[1].len = 1;
        
    // int status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
    // if (status < 0) {
    //     perror("SPI_IOC_MESSAGE Failed");
    //     exit(1);
    // }

    // printf("response(%d): ", status);
    // for(int i = 0; i < 1; i++) {
    //     printf("%02x ", recv[i]);
    // }
    // printf("\n");

    // printf("response: %x %x\n", response1, response2);

}


void RFID_init(
    int     init_spiBusNum,
    char*   init_rstPin,
    int     init_rstGpioNum,
    char*   init_sdaPin
)
{
    spiBusNum   = init_spiBusNum;
    rstPin      = init_rstPin;
    rstGpioNum  = init_rstGpioNum;
    sdaPin      = init_sdaPin;


    configPinForGPIO(rstPin, rstGpioNum, GPIO_OUT);
    gpioWrite(rstGpioNum, 1);
    // gpioWrite(rstGpioNum, 0);

    spiFileDesc = SPI_initPort(spiBusNum, 0);
}



void RFID_test(void)
{
    RFID_init(
        RFID_SPI_NUM,
        RFID_RST_PIN,
        RFID_RST_GPIO_NUM,
        RFID_SDA_PIN
    );

    test1();
}