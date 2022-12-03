// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//
// This software is furnished "as is", without technical support, and with no
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file I2CIO.h
// This file implements a basic IO library using the PCF8574 I2C IO Expander
// chip.
//
// @brief
// Implement a basic IO library to drive the PCF8574* I2C IO Expander ASIC.
// The library implements basic IO general methods to configure IO pin direction
// read and write uint8_t operations and basic pin level routines to set or read
// a particular IO port.
//
//
// @version API 1.0.0
//
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------

#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "I2CIO.h"

extern "C"
{
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}


#define I2C_SMBUS_WRITE 0
#define I2C_SMBUS_BYTE 1
inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
   struct i2c_smbus_ioctl_data args;
   args.read_write = read_write;
   args.command = command;
   args.size = size;
   args.data = data;
   return ioctl(file,I2C_SMBUS,&args);
}
inline __s32 i2c_smbus_write_byte(int file, __u8 value)
{
   return i2c_smbus_access(file,I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE,NULL);
}

// CLASS VARIABLES
// ---------------------------------------------------------------------------


// CONSTRUCTOR
// ---------------------------------------------------------------------------
I2CIO::I2CIO ( )
{
   _i2cAddr     = 0x0;
   _dirMask     = 0xFF;    // mark all as INPUTs
   _shadow      = 0x0;     // no values set
   _initialised = false;
   _fd = 0;
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
int I2CIO::begin ( const char *device, uint8_t i2cAddr )
{
   int r;

   _i2cAddr = i2cAddr;

   _fd = open (device, O_RDWR);
   if (_fd<0)
     return false;

   r=ioctl (_fd, I2C_SLAVE, i2cAddr);
   if (r<0) {
     close(_fd);
     return false;
   }

   _initialised = true;
   return true;
}

//
// pinMode
void I2CIO::pinMode ( uint8_t pin, uint8_t dir )
{
   if ( _initialised )
   {
      if ( OUTPUT == dir )
      {
         _dirMask &= ~( 1 << pin );
      }
      else
      {
         _dirMask |= ( 1 << pin );
      }
   }
}

//
// portMode
void I2CIO::portMode ( uint8_t dir )
{
   if ( _initialised )
   {
      if ( dir == INPUT )
      {
         _dirMask = 0xFF;
      }
      else
      {
         _dirMask = 0x00;
      }
   }
}

//
// read
uint8_t I2CIO::read ( void )
{
// XXX: Read disabled
 return 0;
}

//
// write
int I2CIO::write ( uint8_t value )
{
   int status = 0;

   if ( _initialised )
   {
      // Only write HIGH the values of the ports that have been initialised as
      // outputs updating the output shadow of the device
	_shadow = ( value & ~(_dirMask) );
	status=i2c_smbus_write_byte(_fd, _shadow);
   }
   return ( (status == 0) );
}

//
// digitalRead
uint8_t I2CIO::digitalRead ( uint8_t pin )
{
   uint8_t pinVal = 0;

   // Check if initialised and that the pin is within range of the device
   // -------------------------------------------------------------------
   if ( ( _initialised ) && ( pin <= 7 ) )
   {
      // Remove the values which are not inputs and get the value of the pin
      pinVal = this->read() & _dirMask;
      pinVal = ( pinVal >> pin ) & 0x01; // Get the pin value
   }
   return (pinVal);
}

//
// digitalWrite
int I2CIO::digitalWrite ( uint8_t pin, uint8_t level )
{
   uint8_t writeVal;
   int status = 0;

   // Check if initialised and that the pin is within range of the device
   // -------------------------------------------------------------------
   if ( ( _initialised ) && ( pin <= 7 ) )
   {
      // Only write to HIGH the port if the port has been configured as
      // an OUTPUT pin. Add the new state of the pin to the shadow
      writeVal = ( 1 << pin ) & ~_dirMask;
      if ( level == HIGH )
      {
         _shadow |= writeVal;
      }
      else
      {
         _shadow &= ~writeVal;
      }
      status = this->write ( _shadow );
   }
   return ( status );
}
