char c;
void setup (void)
{


  // turn on SPI in slave mode

  SPCR |= (1<<SPE)|(1<<SPIE);
  SPSR |= (1<<SPIF);
  sei(); // enable global interrupts

  // have to send on master in, *slave out*
  pinMode (MISO, OUTPUT);
  

}  // end of setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{
 c = SPDR;  // grab byte from SPI Data Register

  while(!SPIF){};

 Serial.println(c);

}  // end of interrupt routine SPI_STC_vect

// main loop - wait for flag set in interrupt routine
void loop (void)
{
 

}  // end of loop















#include<SPI.h>

volatile boolean received;
volatile byte receivedData;

ISR (SPI_STC_vect)        //Inerrrput routine function 
{
  receivedData = SPDR;   // Get the received data from SPDR register
  received = true;       // Sets received as True
}

void setup()
{
  Serial.begin(115200);

  pinMode(MISO,OUTPUT);   //Sets MISO as OUTPUT, MISO = Master In Slave Out
  SPCR |= _BV(SPE);       //Turn on SPI in Slave Mode
  received = false;
  SPI.attachInterrupt();  //Activate SPI Interuupt 
}

#define GET_WEIGHT_COMMAND 0x01


void loop()
{ 
    if(received) {  
        switch(receivedData) {
            case 0x01:
                SPDR = rawData MSB;
                break;
            case 0x02:
                SPDR = rawData next most significant byte;
                break;
            case 0x03:
                SPDR = rawData LSB;
                break;
            default:
                break;
        }

    received = false;
    Serial.print(receivedData, HEX);
  }
}








void init(void)
{
    int ArduinoSlaveFileDesc = SPI_initPort(1, 1, DEFAULT Params in SPI.h);
}

float getWeight(void)
{
    #define NUM_BYTES 4
    char sendBuffer[NUM_BYTES] = {0x01, 0x02, 0x03, 0};
    char recvBuffer[NUM_BYTES];

    SPI_transfer(ArduinoSlaveFileDesc, sendBuffer, recvBuffer, NUM_BYTES);

    char weightByte0 = recvBuffer[1];

    #define BITS_PER_BYTE 8

    int rawData = (recvBuffer[1] << 2 * BITS_PER_BYTE) + (recvBuffer[2] << BITS_PER_BYTE) + recvBuffer[3];

    float weightKg = rawData * scalingFactor;

    return weightKg;
}