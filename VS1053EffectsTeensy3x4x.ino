/********************************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  Code adapted by Tobias van Dyk: Arduino Uno  December 2018
                                  Teensy 3x 4x November 2020
 
 ***********************************************************************/
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // This is a fully functional audio effects preamp using the VLSI VS1053b Audio DSP IC. 
 //
 // It has a potentiometer to adjust the volume and the five effect parameters. 
 // It has nine fixed effects and one customizable effect, where each effect has five effects settings namely delay, 
 // decay-repeat, modulation speed and depth, and the mix-ratio of the processed and direct audio. It includes adjustments 
 // for bass and treble boost, bass and treble centre frequency, a selection of six input gain values, an option for saving 
 // or retrieving the current/saved parameters to/from the Teensy Eeprom-simulated, and a basic/normal/advanced/edit menu option 
 // that determines the number of functions that are cycled through. Adjustments are made using three pushbuttons, namely a function 
 // select button and two buttons to increase and decrease values for the selected function .
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>
#include <EEPROM.h>
#include <Bounce.h>

// Connect SCLK, MISO and MOSI to standard hardware SPI pins. 
#define SCLK 13       // SPI Clock shared with SD card
#define MISO 12       // Input data from vs1053 or SD card
#define MOSI 11       // Output data to vs1053 or SD card

// These are the pins used for the Adafruit vs1053B breakout module
#define XRST  9                // vs1053 reset (output)
#define XCS   10               // vs1053 chip select (output)
#define XDCS  8                // vs1053 Data select (output)
#define XDREQ 3                // vs1053 Data Ready an Interrupt pin (input)
#define SDCS  BUILTIN_SDCARD   // Use Teensy built-in card
// For Teensy 3.5, 3.6, 4.0, 4.1 better to use its built-in SDCard
//#define SDCS 4                // Use vs1053 SDCard Card chip select pin

/////////////////////////////////////////////////////////////////////////////////
// vs1053B.h
/////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>

#define vs1053_SCI_READ 0x03
#define vs1053_SCI_WRITE 0x02

#define vs1053_REG_MODE  0x00
#define vs1053_REG_STATUS 0x01
#define vs1053_REG_BASS 0x02
#define vs1053_REG_CLOCKF 0x03
#define vs1053_REG_DECODETIME 0x04
#define vs1053_REG_AUDATA 0x05
#define vs1053_REG_WRAM 0x06
#define vs1053_REG_WRAMADDR 0x07
#define vs1053_REG_HDAT0 0x08
#define vs1053_REG_HDAT1 0x09
#define vs1053_REG_VOLUME 0x0B

#define vs1053_GPIO_DDR 0xC017
#define vs1053_GPIO_IDATA 0xC018
#define vs1053_GPIO_ODATA 0xC019

#define vs1053_INT_ENABLE  0xC01A

#define vs1053_MODE_SM_DIFF 0x0001
#define vs1053_MODE_SM_LAYER12 0x0002
#define vs1053_MODE_SM_RESET 0x0004
#define vs1053_MODE_SM_CANCEL 0x0008
#define vs1053_MODE_SM_EARSPKLO 0x0010
#define vs1053_MODE_SM_TESTS 0x0020
#define vs1053_MODE_SM_STREAM 0x0040
#define vs1053_MODE_SM_SDINEW 0x0800
#define vs1053_MODE_SM_ADPCM 0x1000
#define vs1053_MODE_SM_LINE1 0x4000
#define vs1053_MODE_SM_CLKRANGE 0x8000

#define vs1053_SCI_AIADDR 0x0A
#define vs1053_SCI_AICTRL0 0x0C
#define vs1053_SCI_AICTRL1 0x0D
#define vs1053_SCI_AICTRL2 0x0E
#define vs1053_SCI_AICTRL3 0x0F

////////////////////////////////////////////////////////////////////////////////
// vs1053B.cpp
////////////////////////////////////////////////////////////////////////////////
#define vs1053_CONTROL_SPI_SETTING  SPISettings(250000,  MSBFIRST, SPI_MODE0) // Control: 2.5 MHz SPI speed  
//#define vs1053_DATA_SPI_SETTING   SPISettings(8000000, MSBFIRST, SPI_MODE0) // Data: 8 MHz SPI speed


//////////////////////////////////////////////////
void vs1053SetVolume(uint8_t left, uint8_t right) 
{ uint16_t v;
  v = left;
  v <<= 8;
  v |= right;

  cli();
  vs1053SciWrite(vs1053_REG_VOLUME, v);
  sei();
}

///////////////////////
void vs1053SoftReset() 
{ vs1053SciWrite(vs1053_REG_MODE, vs1053_MODE_SM_SDINEW | vs1053_MODE_SM_RESET);
  delay(100);
}

//////////////////////////
void vs1053Reset() 
{ if (XRST >= 0) 
     { digitalWrite(XRST, LOW);
       delay(100);
       digitalWrite(XRST, HIGH);
     }
  digitalWrite(XCS, HIGH);
  digitalWrite(XDCS, HIGH);
  delay(100);
  vs1053SoftReset();
  delay(100);

  vs1053SciWrite(vs1053_REG_CLOCKF, 0x6000);

  vs1053SetVolume(40, 40);
}

//////////////////////
uint8_t vs1053Begin() 
{ if (XRST >= 0) { pinMode(XRST, OUTPUT);  // if reset = -1 ignore
                             digitalWrite(XRST, LOW);
                           }

  pinMode(XCS, OUTPUT);
  digitalWrite(XCS, HIGH);
  pinMode(XDCS, OUTPUT);
  digitalWrite(XDCS, HIGH);
  pinMode(XDREQ, INPUT);

  // SPI.beginTransaction(SPISettings(clockspeed, MSBFIRST, SPI_MODE0))
  // Begin using the SPI bus. Normally this is called before asserting the chip select signal. 
  // The SPI is configured to use the clock, data order (MSBFIRST or LSBFIRST) and data mode (SPI_MODE0, SPI_MODE1, SPI_MODE2, 
  // or SPI_MODE3). The clock speed should be the maximum speed the SPI slave device can accept. 
  SPI.begin();
  //SPI.beginTransaction(SPISettings(250000,  MSBFIRST, SPI_MODE0));
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV128); 
  
  vs1053Reset();

  return (vs1053SciRead(vs1053_REG_STATUS) >> 4) & 0x0F;
}

/////////////////////////////////////
uint16_t vs1053SciRead(uint8_t addr) 
{ uint16_t data;
  SPI.beginTransaction(vs1053_CONTROL_SPI_SETTING);
  digitalWrite(XCS, LOW);  
  vs1053SpiWrite(vs1053_SCI_READ);
  vs1053SpiWrite(addr);
  delayMicroseconds(10);
  data = vs1053SpiRead();
  data <<= 8;
  data |= vs1053SpiRead();
  digitalWrite(XCS, HIGH);
  SPI.endTransaction();
  return data;
}

//////////////////////////////////////////////
void vs1053SciWrite(uint8_t addr, uint16_t data) 
{ SPI.beginTransaction(vs1053_CONTROL_SPI_SETTING);
  digitalWrite(XCS, LOW);  
  vs1053SpiWrite(vs1053_SCI_WRITE);
  vs1053SpiWrite(addr);
  vs1053SpiWrite(data >> 8);
  vs1053SpiWrite(data & 0xFF);
  digitalWrite(XCS, HIGH);
  SPI.endTransaction();
}

////////////////////////
uint8_t vs1053SpiRead()
{ int8_t x;
  x = 0;
  // Transmit a byte from master to slave, and simultaneously receive a byte from slave to master. 
  // SPI always transmits and receives at the same time, but often the received byte is ignored. 
  // When only reception is needed, 0 or 255 is transmitted to cause the reception. 
  x = SPI.transfer(0x00);

  return x;
}

///////////////////////////////
void vs1053SpiWrite(uint8_t c)
{ SPI.transfer(c);
}

/////////////////////////////////////
// End .cpp
/////////////////////////////////////


#define OLED_RESET -1     // GPIO None
Adafruit_SSD1306 display(OLED_RESET);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The VLSI effects processing code is loaded as a plugin within the Arduino sketch.
// Further details of the VLSI effects processing can be obtained by installing their development tool
// VSIDE and then opening the folder VSIDE\templates\project\VS10X3_Audio_Effects. 
// Use their Coff2All tool to convert the executable file into a C code type plugin which was then copied into the Arduino sketch 
// and which loads before the loop function of the sketch starts. 
// Thee location of the effects variables in their example code was changed in the three files: effect.c and int.c and a4plugin.c. 
// The latter is the plugin code that is loaded from the Arduino sketch.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const static word plugin[907] = { /* Compressed plugin */
        0x0007,0x0001, /*copy 1*/
	0x8050,
	0x0006,0x036e, /*copy 878*/
	0x0030,0x0697,0x3700,0x0024,0x0000,0x0801,0xc010,0x0024,
	0x3f00,0x0024,0x2800,0x15c0,0x0006,0x2016,0x0038,0x0011,
	0x0038,0x0002,0x0000,0x2197,0xb886,0x184c,0xb88c,0x3848,
	0x3e05,0xb814,0x2912,0x1300,0x3685,0x0024,0x0007,0x9250,
	0x0000,0x0000,0xc890,0x2000,0x0002,0x0010,0x3000,0x4024,
	0x0002,0x0090,0xa100,0x0024,0x6092,0x0000,0x4010,0x0024,
	0xb882,0x2000,0x0008,0x0000,0x2915,0x7ac0,0x0038,0x0010,
	0x2912,0x1180,0x0000,0x0024,0x0018,0x0000,0x3613,0x0024,
	0xb880,0x3840,0x3e10,0x0024,0x0017,0x7000,0x2900,0x65c0,
	0x3e00,0x0024,0x0000,0x0084,0x36e3,0x0024,0x2908,0xbe80,
	0x0017,0x7005,0x0030,0x0690,0x0000,0x0241,0x3000,0x0024,
	0xc010,0x0024,0xb882,0x2000,0x0006,0xa050,0x0000,0x0000,
	0x3009,0x2000,0x0006,0xa010,0x0000,0x2000,0x3009,0x2000,
	0x0004,0x0000,0x2915,0x7ac0,0x0000,0x0010,0x0030,0x05d0,
	0x0000,0x0401,0x3000,0x0024,0xc010,0x0024,0x003f,0xfdc1,
	0x3800,0x0024,0x3000,0x0024,0xb010,0x0024,0x3800,0x0024,
	0x0030,0x02d0,0x0000,0x4040,0x2912,0x1300,0x3800,0x0024,
	0x2912,0x0ec0,0x3613,0x0024,0xf400,0x4001,0x0000,0x2000,
	0x6100,0x0024,0x0002,0x0000,0x2800,0x4509,0x0002,0x0010,
	0x0000,0x2000,0x3473,0x184c,0xf400,0x4510,0xf400,0x4512,
	0xf400,0x4513,0x2912,0x1f80,0x3493,0x0024,0x0030,0x0610,
	0x0000,0x0200,0x3000,0x4024,0xb100,0x0024,0x0002,0x0150,
	0x2800,0x42c5,0x003f,0xfbc1,0x0002,0x0110,0x0000,0x0000,
	0x3800,0x0024,0x0002,0x01d0,0x3800,0x0024,0x0030,0x0650,
	0x3000,0x0024,0xb010,0x0024,0xb882,0x2000,0x0000,0x1000,
	0x6100,0x0024,0x0002,0x0050,0x2800,0x3c01,0x003f,0xffc0,
	0x6030,0x8404,0xfe88,0x0024,0x48ba,0x0024,0x4580,0x4444,
	0x003f,0xffc5,0x6458,0x044c,0x0002,0x0210,0x2800,0x30c8,
	0xb888,0x0024,0x0038,0x0011,0x6898,0x0024,0x3801,0x104c,
	0x0002,0x0110,0x3009,0x3041,0x3009,0x33d1,0x3c30,0x8404,
	0x3c00,0xb3c3,0xfeb8,0x4491,0x48bb,0xc637,0x4058,0x03c0,
	0x001f,0x4005,0x6502,0xb087,0xfe9e,0x044c,0x48b6,0x0c8c,
	0xfe88,0x0024,0x1fff,0xfc44,0x4db6,0x0024,0xf400,0x40c3,
	0xad46,0x0024,0x3a00,0x8024,0x3200,0x4024,0x3000,0x0024,
	0xfe82,0x3004,0x48b6,0xb385,0x6500,0x1006,0x3009,0x1087,
	0xfe8e,0x13c4,0x4db6,0x0880,0xf400,0x40c3,0xad4a,0x1002,
	0xf400,0x4090,0x3900,0x1383,0x003f,0xffc0,0x3009,0x2004,
	0x6200,0x93d1,0x3009,0x13c1,0x2800,0x3b48,0x0002,0x0250,
	0x0038,0x0002,0x6890,0x0024,0x3800,0x0024,0x2800,0x2cc0,
	0x6192,0x0024,0x0000,0x0000,0x2800,0x3ac0,0x6294,0x2000,
	0x0002,0x0012,0x3010,0x0024,0x460c,0x0800,0xf062,0x0024,
	0xfe82,0x0000,0x48ba,0x0024,0x4252,0x4103,0x6100,0x0024,
	0x0038,0x0001,0x6018,0x0024,0x0002,0x0190,0x2800,0x4258,
	0xb882,0x0024,0x6892,0x0024,0x3800,0x4024,0x0008,0x0001,
	0x4010,0x0024,0xf400,0x4011,0x0000,0x1000,0x3473,0x184c,
	0x3e10,0x0024,0x290c,0x7300,0x3e15,0x124c,0x2800,0x2500,
	0x36e3,0x0024,0x2800,0x4040,0x3800,0x4024,0x30f0,0x0024,
	0x6890,0x2000,0x0002,0x01d0,0x3800,0x0024,0x0030,0x0650,
	0x0000,0x0400,0x3000,0x4024,0x2800,0x2c80,0xc100,0x0024,
	0x3000,0x4024,0x6018,0x0024,0x0002,0x0052,0x2800,0x4681,
	0x0000,0x0024,0x3800,0x0024,0x0001,0x0000,0x3200,0x4024,
	0x6018,0x0024,0x0000,0x0024,0x2800,0x4841,0x0000,0x0024,
	0x3a00,0x0024,0x0002,0x0112,0x001f,0x4000,0x3200,0x4024,
	0x6018,0x0024,0x0000,0x0024,0x2800,0x4a41,0x0000,0x0024,
	0x3a00,0x0024,0x0002,0x00d2,0x001f,0x4000,0x3200,0x4024,
	0x6010,0x0024,0x0000,0x0024,0x2800,0x2501,0x0000,0x0024,
	0x001f,0x4000,0x2800,0x2500,0x3a00,0x0024,0x3e12,0xb817,
	0x3e12,0x7808,0x3e11,0xb811,0x3e15,0x7810,0x3e18,0xb823,
	0x3e18,0x3821,0x3e10,0x3801,0x48b2,0x0024,0x3e10,0x3801,
	0x3e11,0x3802,0x3009,0x3814,0x0030,0x0717,0x3f05,0xc024,
	0x0030,0x0351,0x3100,0x0024,0x4080,0x0024,0x0030,0x10d1,
	0x2800,0x57c5,0x0001,0x800a,0x0006,0x6514,0x3111,0x8024,
	0x6894,0x13c1,0x6618,0x0024,0xfe44,0x1000,0x4cb2,0x0406,
	0x3c10,0x0024,0x3c50,0x4024,0x34f0,0x4024,0x661c,0x1040,
	0xfe64,0x0024,0x4cb2,0x0024,0x3cf0,0x4024,0xbc82,0x3080,
	0x0030,0x0351,0x3100,0x8024,0xfea8,0x0024,0x5ca2,0x0024,
	0x0000,0x0182,0xac22,0x0024,0xf7c8,0x0024,0x48b2,0x0024,
	0xac22,0x0024,0x2800,0x5b40,0xf7cc,0x1002,0x0030,0x0394,
	0x3400,0x4024,0x3100,0x184c,0x0006,0xc051,0x291e,0x8080,
	0x0006,0x6410,0x4088,0x1001,0x0030,0x1111,0x3100,0x184c,
	0x0006,0xc051,0x291e,0x8080,0x0006,0x6550,0x0006,0x6694,
	0x408c,0x1002,0xf224,0x0024,0x0006,0xa017,0x2800,0x5f55,
	0x0000,0x0024,0x2800,0x6041,0x0006,0x6410,0x3050,0x0024,
	0x3000,0x4024,0x6014,0x0024,0x0000,0x0024,0x2800,0x5e99,
	0x0000,0x0024,0xf400,0x4040,0x38b0,0x0024,0x2800,0x6040,
	0x3800,0x0024,0x2800,0x6001,0x0000,0x0024,0xf400,0x4106,
	0xf400,0x4184,0x0030,0x06d5,0x3d05,0x5bd4,0xf400,0x4115,
	0x36f1,0x1802,0x36f0,0x1801,0x2210,0x0000,0x36f0,0x1801,
	0x36f8,0x1821,0x36f8,0x9823,0x0006,0x9f57,0x0020,0xffd0,
	0x3009,0x1c11,0x3985,0x4024,0x3981,0x8024,0x3009,0x3c11,
	0x36f5,0x5810,0x36f1,0x9811,0x36f2,0x5808,0x3602,0x8024,
	0x0030,0x0717,0x2100,0x0000,0x3f05,0xdbd7,0x3613,0x0024,
	0x3e12,0x0024,0x3e05,0xb814,0x3615,0x0024,0x3e00,0x3841,
	0x3e00,0xb850,0x0007,0x9250,0x3e14,0x7812,0xb880,0x930c,
	0x3800,0x0024,0x0030,0x00d0,0x3440,0x0024,0x2908,0x6400,
	0x3800,0x0024,0x003f,0xfcc1,0x0030,0x0050,0x3000,0x134c,
	0xb010,0x0024,0x38f0,0x0024,0x3430,0x8024,0x6284,0x0000,
	0x0010,0x0001,0x2800,0x8145,0x0000,0x0024,0x002f,0xffc1,
	0xb010,0x0024,0x3800,0x0024,0x000b,0x4000,0x0006,0xc490,
	0x2908,0x7f80,0x3009,0x2000,0x0015,0xbd51,0x0030,0x0ad0,
	0x3800,0x0024,0x0000,0x01c0,0x2915,0x8300,0x0006,0xc050,
	0x0000,0x0300,0xb882,0x0024,0x2914,0xbec0,0x0006,0x6410,
	0x0001,0x0000,0x0030,0x0350,0x6284,0x2080,0x0000,0x0100,
	0x2800,0x7245,0x3800,0x0024,0xf202,0x0000,0xc010,0x0024,
	0x3800,0x0024,0x0030,0x0390,0x3000,0x0024,0x6080,0x0024,
	0x0006,0x6552,0x2800,0x7495,0x1fe0,0x0040,0x003f,0xffc0,
	0x3800,0x0024,0x1fe0,0x0040,0x0006,0x6410,0x3870,0x138c,
	0x3a00,0x00cc,0x0000,0x00c0,0x0030,0x03d2,0x3200,0x4024,
	0xb100,0x0024,0x3800,0x0024,0x002e,0xe000,0x3420,0x8024,
	0x6200,0x0024,0x0000,0x0140,0x2800,0x80c5,0x0030,0x1090,
	0x0017,0x7000,0x6200,0x0024,0x0000,0x01c0,0x2800,0x8045,
	0x0000,0x0024,0x000b,0xb800,0x6204,0x0024,0x0000,0x01c0,
	0x2800,0x7c15,0x0000,0x0024,0x3800,0x0024,0x0000,0x0080,
	0x0030,0x0050,0x3000,0x4024,0xc100,0x0024,0x3800,0x0024,
	0x0000,0x0000,0x0006,0x9f90,0x3009,0x2000,0x0006,0x9f50,
	0xb880,0xa000,0x0006,0x9fd0,0x3009,0x2000,0x36f4,0x5812,
	0x36f0,0x9810,0x36f0,0x1801,0x3405,0x9014,0x36f3,0x0024,
	0x36f2,0x0024,0x2000,0x0000,0x0000,0x0024,0x0000,0x01c0,
	0x0030,0x1090,0x2800,0x7c00,0x3800,0x0024,0x2800,0x7c00,
	0x3800,0x0024,0x2800,0x6c40,0xc010,0x0024,
	0x0007,0x0001, /*copy 1*/
	0x0800,
	0x0006,0x0006, /*copy 6*/
	0x0000,0x0000,0x1f40,0x6d60,0x3e80,0x3e80,
	0x0006, 0x8004, 0x0000, /*Rle(4)*/
	0x0007,0x0001, /*copy 1*/
	0x8023,
	0x0006,0x0002, /*copy 2*/
	0x2a00,0x4cce,
	0x000a,0x0001, /*copy 1*/
	0x0050};
#define PLUGIN_SIZE 907

const static byte Scale[256] = {   0,  1,  3,  5,  7,  9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 31,
                                  60, 80,100,120,140,160,180,200,217,217,217,217,220,220,220,220,
			                           230,230,230,230,230,230,230,230,235,235,235,235,235,235,235,235,
                                 236,236,236,236,236,236,236,236,237,237,237,237,237,237,237,237,
                                 238,238,238,238,238,238,238,238,239,239,239,239,239,239,239,239,
                                 240,240,240,240,240,240,240,240,240,240,240,241,241,241,241,241,
                                 241,241,242,242,242,242,242,242,242,242,242,242,244,244,244,244,
                                 244,244,244,244,244,244,244,244,244,244,244,244,246,246,246,246,
                                 246,246,246,246,246,246,246,246,246,246,246,246,248,248,248,248,
                                 248,248,248,248,248,248,248,248,248,248,248,248,249,249,249,249,
                                 249,249,249,249,249,249,249,249,249,249,249,249,249,249,249,249,
                                 250,250,250,250,250,250,250,250,250,250,250,250,251,251,251,251,
                                 251,251,251,251,251,251,251,251,251,251,251,251,252,252,252,252,
                                 252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,253,
                                 253,253,253,253,253,253,253,253,253,253,253,253,254,254,254,254,
                                 254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,254 };

const static int GainValue[6] = {512,1024,2048,4096,8192,16384};

const static word EffectsParam[60] = {0,0,8001,28000,16000,16000,
                                      500,30,40,20000,16000,16000,
                                      0,0,0,28000,16000,16000,
                                      0,0,2000,20000,16000,16000,
                                      500,30,500,20000,16000,16000,
                                      800,50,400,20000,16000,16000,
                                      0,0,8000,28000,16000,16000,
                                      0,0,8001,28000,0,0,
                                      50,50,50,50,18000,18000,
                                      0,0,0,0,0,0};

uint16_t cp9[6] = {0, 0, 0, 0, 0, 0}; // Effects Parameters Custom

uint16_t p09[6] = {0, 0, 0, 0, 0, 0}; // Effects Parameters = EffectsParam[0-8]

const static byte Menu3[6]    =    { 9,10,11,12,13,14 };
const static byte Menu2[14]   =    { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14 }; 
const static byte Menu1[10]   =    { 1, 2, 3, 4, 9,10,11,12,13,14 }; 
const static byte Menu0[5]    =    { 1, 2, 3, 4, 9 };
const static byte MNumber[4]  =    { 5, 10, 14, 6 };
const static byte MNumber9[4] =    { 4, 4, 8, 0 };
byte mPos = 0;
byte mNum = 14;

// Create Bounce objects for each button
const int btnFunc1 = 17;          // D17 Select Function
const int btnNext1 = 16;          // D16 Up Button
const int btnPrev1 = 15;          // D15 Down Button
Bounce btnFunc = Bounce(btnFunc1, 10);  // 10 ms debounce
Bounce btnNext = Bounce(btnNext1, 10);  
Bounce btnPrev = Bounce(btnPrev1, 10); 

//#define btnFunc A3          // D17 Select Function
//#define btnNext A2          // D16 Up Button
//#define btnPrev A1          // D15 Down Button
#define ActivityLED1 A14    // D10 Show Activity
#define ActivityLED2 A15    // D10 Show Activity
#define ActivityLED3 A16    // A1 or D15 Show Activity

byte Volume = 0x0101;
byte Effect = 0;
int Bass = 0;     	// 0 - 15  All used to generate 16 bit int SCI_BASS
int Treble = 0;   	// 0 - 15
int BassFreq = 6;       // 2 - 15
int TrebleFreq = 10;    // 1 - 15
byte Gain = 1;          // 0 - 5 0.5x to 4x translates to 512 to 16384
byte Menu = 2;          // Start with Advanced Menu = 2  0 = Basic Menu 1 = Intemediate Menu 3 Edit Menu
  
//booleans to activate when btnpressed
boolean AnyBtnPress = false;
int Pot0, Pot1 = 0;     // Values 0 to 1023
int Pot0Prev = 1;
byte EffectDone = 1;
byte EepromDone = 0;

// E Effect 1 V volume 2 B bass 3 T treble 4 F bassfreq 5 F treblefreq 6 
// G gain 7 M = S/W SaveWriteEeprom 8 f Menu Basic/Mid/Full 9 E Effect9 10 to 14 
const static char Function[17] = " EVBTFFGSfEEEEE ";  // Function Indicator V Volume 
byte FunctionNum = 1;                                         // 1 is Effect 2 is Volume
uint16_t loopVar1, loopVar2 = 0;

const char *PP00[] = {"Depth", "Speed", "Delay", "Decay", "Mix", "Bass", "Treble", "Freq  Hz", "Freq  kHz",
                      "0.5x Gain", "1x Gain", "2x Gain", "3x Gain", "4x Gain", "5x Gain", 
                      "Functions", "Basic", "Normal", "Full", "Edit", "Up Save", "Down Read", "Saved", "Read", "Error"};

////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////// 
void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  display.clearDisplay();
  display.display();

  // Configure the pins for input mode with pullup resistors.
  pinMode(btnNext1, INPUT_PULLUP); // Pin 0
  pinMode(btnPrev1, INPUT_PULLUP); // Pin 1
  pinMode(btnFunc1, INPUT_PULLUP); // Pin 4 Rotary Encoder Push Switch
  
  //pinMode (btnNext, INPUT);
  //pinMode (btnPrev, INPUT);
  //pinMode (btnFunc, INPUT); // FunctionNum = 0

  pinMode (ActivityLED1, OUTPUT);
  digitalWrite (ActivityLED1, LOW);
  pinMode (ActivityLED2, OUTPUT);
  digitalWrite (ActivityLED2, LOW);

  AnyBtnPress = false;

  //WriteEeprom0();            // This tested successfully with Test = 195
  //Test = EEPROM.read(0);   

  vs1053Begin(); // initialise the music player
 
  vs1053SciWrite(vs1053_REG_BASS, 0x0a0a);
  vs1053SetVolume(1,1);

  LoadUserCode(); // This will start effects.c
  
  mNum = 14;
  ReadEffects();  
        
  DoDisplay();
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void loop() 
{ 
  int m = 8192;
  byte n = 0;
  
  Pot0 = analogRead(A0); //use A0 to read the electrical signal
  delay (10);
  Pot1 = analogRead(A0); 
  Pot0 = (Pot1+Pot0)/2;

  //check if the buttons are being pressed
  AnyBtnPress = false;
  if (btnNext.update()) AnyBtnPress = true;
  if (btnPrev.update()) AnyBtnPress = true;
  if (btnFunc.update()) AnyBtnPress = true;
  
  //if (btnNext.update())||(btnPrev.update())||(btnFunc.update()) AnyBtnPress = true; else AnyBtnPress = false;
   
  // Check each button for "falling" edge.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - button connects pin to ground)
  if (AnyBtnPress) readBtn();
   
  ////////////////////////////////////////////////////////////////////////////////////////////////
  // depth = depth of modulation (effect to delay length) 0  - 1023
  // speed = speed of modulation range 0 .. 1023 (5.7Hz)
  // delay = pre-delay (depth adds to this delay) 0 to 8191
  // decay = amount of feedback from output to input ("repeat" control for delay effect) 0 - 32000
  // mix   = mix of dry and effect signal (0 == all dry, 32000 == all effect)
  //    10  Change Depth 
  //    11  Change Speed 
  //    12  Change Delay
  //    13  Change Decay 
  //    14  Change Mix 
  //   (15) Mix Original = Mix
  ////////////////////////////////////////////////////////////////////////////////////////////////
  if (Pot0Prev != Pot0)                             // Sequence = .V.......EEEEE 
     { Pot0Prev = Pot0; 
       if (FunctionNum==2)  ChangeVol(2);
       
       if (Menu>0)                                                                  // Skip the 5 params changes if = 0
          { if (FunctionNum>9) n = FunctionNum-10;
            if ((FunctionNum==10)||(FunctionNum==11)) cp9[n] = Pot0;                // 0-1023 
            if (FunctionNum==12) m = 8192;                                          // 0-8192
            if (FunctionNum>12)  m = 32000;                                         // 0-32000
            if (FunctionNum>11)  cp9[n] = map(Pot0, 0, 1023, 0, m);  
            if (FunctionNum==14) { cp9[5] = cp9[4]; p09[5] = cp9[4]; }              // MixOrig = Mix
            if (FunctionNum>9) { p09[n] = cp9[n]; EffectDone = 0;  Effect = 9; }
            //if (FunctionNum>9) { p09[n] = cp9[n]; EffectDone = 0; }
          } 
       }
         
  DoDisplay(); 
  delay(100); 
  ReadLoopVar();
     
  if (EffectDone==0) 
     { WriteEffects ();    
       EffectDone = 1;
     }  
 
}

////////////////////////////////////////////////////////////
// From VLSI Application Notes
////////////////////////////////////////////////////////////
void LoadUserCode(void) {
  // User application code loading tables for VS10xx
  // See http://www.vlsi.fi/en/support/software/vs10xxpatches.html

  int i = 0;
  while (i< PLUGIN_SIZE) {
    word addr, n, val;
    addr = plugin[i++];
    n = plugin[i++];
    if (n & 0x8000U) { /* RLE run, replicate n samples */
      n &= 0x7FFF;
      val = plugin[i++];
      while (n--) {
        vs1053SciWrite (addr , val);       
      }
    } 
    else {           /* Copy run, copy n samples */
      while (n--) {
        val = plugin[i++];
        vs1053SciWrite (addr , val);        
      }
    }
  }
  return;
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void ReadLoopVar()
{ uint16_t addr1 = 0x0806;
  uint16_t addr2 = 0x0807;
vs1053SciWrite(vs1053_REG_WRAMADDR, addr1);
loopVar1 = vs1053SciRead(vs1053_REG_WRAM);
if (loopVar1==1) digitalWrite (ActivityLED1, HIGH);
        else digitalWrite (ActivityLED1, LOW); 
vs1053SciWrite(vs1053_REG_WRAMADDR, addr2);
loopVar2 = vs1053SciRead(vs1053_REG_WRAM);
if (loopVar2==1) digitalWrite (ActivityLED2, LOW);
        else digitalWrite (ActivityLED2, HIGH);
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void ReadEffects()
{ uint16_t addr1 = 0x0800;
  for (int n = 0; n<6; n++)
      { vs1053SciWrite(vs1053_REG_WRAMADDR, addr1);
        p09[n] = vs1053SciRead(vs1053_REG_WRAM);
        addr1++; // = 0x0801 to 0x0805 = Mix Original
      }
}
////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void WriteEffects()
{ uint16_t addr1 = 0x0800;
  uint16_t pp1to8 = 0;
  uint16_t n, i = 0;
  n = 6*Effect; // 6 parameters per effect - two are the same
  for (i = 0; i<6; i++)
      { pp1to8 = EffectsParam[n];
        vs1053SciWrite(vs1053_REG_WRAMADDR, addr1);
        if (Effect<9) vs1053SciWrite(vs1053_REG_WRAM, pp1to8);
                 else vs1053SciWrite(vs1053_REG_WRAM, cp9[i]); 
        n++;
        addr1++; // = 0x0801 to 0x0805
      }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
// E Effect 1 V volume 2 B bass 3 T treble 4 F bassfreq 5 F treblefreq 6 
// G gain 7 M = S/W SaveWriteEeprom 8 f Functions Basic/Mid/Full 9 E Effect9 10 to 14 
// const static char Function[17] = " EVBTFFGSfEEEEE ";  // Function Indicator V Volume 
////////////////////////////////////////////////////////////////////////////////////////////////
void DoDisplay ()
{ char Chr;
  int dp4 = p09[3]/1000;
  int dp5 = p09[4]/1000;

  //byte RealVol = (0xFE - Volume)/5 - 40; // Max Volume is 0 now it is 10
  //if (RealVol>100) RealVol = 0;
  byte RealVol = map(Volume, 0x00, 0xFE, 100, 0);
  /////////////////////////////////////////////////////////////
  // Large Font one line 
  /////////////////////////////////////////////////////////////
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,12);
  display.clearDisplay();
  Chr = Function[FunctionNum];
  display.print(Chr);
  if (FunctionNum<10) display.print(" "); 

  if (FunctionNum==1)  display.print(Effect);
  if (FunctionNum==2)  display.print(RealVol);
  if (FunctionNum==3)  display.print(Bass);
  if (FunctionNum==4)  display.print(Treble);
  if (FunctionNum==5)  display.print(BassFreq*10);
  if (FunctionNum==6)  display.print(TrebleFreq);
  if (FunctionNum==7)  display.print(Gain);
  if (FunctionNum==8)  display.print("R");
  if (FunctionNum==9)  display.print(Menu);
  
  if (FunctionNum>9)  display.print("9");
  /////////////////////////////////////////////////////////////
  // Small Font four lines 
  /////////////////////////////////////////////////////////////
  display.setFont();
  display.println(); 
  
  if (FunctionNum<5)                           // 1-4 Effects Volume Bass Treble
     { display.print("Effect ");
       display.println(Effect);
       display.print(p09[0]); 
       display.print(" ");
       display.println(p09[1]); 
       display.println(p09[2]); 
       display.print(dp4); 
       display.print("k ");
       display.print(dp5); 
       display.print("k ");
     }
     
  
  if ((FunctionNum==5)||(FunctionNum==6))     // 5 6 Bass Treble Freq
     { display.println(); 
       display.println(PP00[FunctionNum]);
       display.print(PP00[FunctionNum+2]);
     }  
  
  if (FunctionNum==7)                         // 7 Gain
     { display.println();
       display.print(PP00[FunctionNum+2+Gain]);
     } 

  if (FunctionNum==8)                         // 8 Save Read
     { display.println();
       if (EepromDone==0)
          { display.println(PP00[FunctionNum+12]);
            display.print(PP00[FunctionNum+13]);
          }
       if (EepromDone>0) 
          { display.println(PP00[FunctionNum+13+EepromDone]);  }
     } 
     
  if (FunctionNum==9)                         // 9 Basic - Advanced Functions 0 1 2 3
     { display.println();
       display.println(PP00[FunctionNum+6]);
       display.print(PP00[FunctionNum+7+Menu]);
     } 
        
  if ((FunctionNum>9)&&(FunctionNum<15)) 
     { display.println(); 
       display.println(PP00[FunctionNum-10]);
       display.print(cp9[FunctionNum-10]);
     }
      
  display.display();
      
}
///////////////////////////////////////////////////
//
///////////////////////////////////////////////////
void ChangeEffect(byte UpDwn) // Effect = 1 to 9  0 is down 1 is up
{
  if (UpDwn==0) { if (Effect > 0) Effect = Effect - 1; else Effect = 9; }  // Wrap-around  
  if (UpDwn==1) { if (Effect < 9) Effect = Effect + 1; else Effect = 0; }  
  WriteEffects(); 
  ReadEffects();     
}
/////////////////////////////////////////////////////////////////
// Send left and right as bytes
// 0 Loud FE Quiet FF  is reset analog
//////////////////////////////////////////////////////////////////
void ChangeVol(byte UpDwn) // 0 is down 1 is up 2 is Pot adjust
{
   if (UpDwn==2)
      { Volume = Pot0/4;
        Volume = Scale[Volume];
        Volume = 0xFF - Volume;
        if (Volume==0xFF) Volume = 0xFE;
      }

  //If Up and Down Buttons used  
  //if (UpDwn==0) { if (Volume<0x2F) Volume = Volume + 1; if (Volume==0x2F) Volume = 0xFE; }      // Softer no wrap-around
  //if (UpDwn==1) { if (Volume==0xFE) Volume = 0x2F; else if (Volume>0x00) Volume = Volume - 1; } // Louder
  
  vs1053SetVolume(Volume,Volume); 
     
}
///////////////////////////////////////////////////
//(SCI_BASS) = 0xfa7a; 
// a=> 10khz a => 100 hz f = full +15db 7 => +10db
// sciWrite(vs1053_REG_BASS, 0);  // clear Bass
///////////////////////////////////////////////////
void ChangeBass(byte UpDwn) // 0 is down 1 is up
{    
     uint16_t Bass1, Bass2 = 0;
     uint16_t Bass3 = 0xff0f;
     if (UpDwn==0) {if (Bass>0x0) Bass = Bass - 1; } 
     if (UpDwn==1) {if (Bass<0xF) Bass = Bass + 1; } 
     Bass1 = vs1053SciRead(vs1053_REG_BASS);
     Bass2 = Bass << 4; 
     Bass1 = Bass1 & Bass3; // clear bits 4 to 7 
     Bass1 = Bass1 | Bass2; // set bits 4 to 7 = bass2
     vs1053SciWrite(vs1053_REG_BASS, Bass1);  
     
}
///////////////////////////////////////////////////
// Send left and right as bytes
// Volume word v left is MSB right is LSB
// 0 Loud FE Quiet FF  is reset analog
///////////////////////////////////////////////////
void ChangeTreble(byte UpDwn) // 0 is down 1 is up
{    
     uint16_t Treble1, Treble2 = 0;
     uint16_t Treble3 = 0x0fff;
     if (UpDwn==0) {if (Treble>0x0) Treble = Treble - 1; } 
     if (UpDwn==1) {if (Treble<0xF) Treble = Treble + 1; } 
     Treble1 = vs1053SciRead(vs1053_REG_BASS);
     Treble2 = Treble << 12; 
     Treble1 = Treble1 & Treble3; // clear bits 12 to 15 
     Treble1 = Treble1 | Treble2; // set bits 12 to 15 = treble2
     vs1053SciWrite(vs1053_REG_BASS, Treble1);  
     
}
/////////////////////////////////////////////////////////////
// 1 to 15 kHz
/////////////////////////////////////////////////////////////
void ChangeTrebleFreq(byte UpDwn)
{    
     uint16_t Treble1, Treble2 = 0;
     uint16_t Treble3 = 0xf0ff;
     if (UpDwn==0) {if (TrebleFreq>1)  TrebleFreq = TrebleFreq - 1; } 
     if (UpDwn==1) {if (TrebleFreq<15) TrebleFreq = TrebleFreq + 1; } 
     Treble1 = vs1053SciRead(vs1053_REG_BASS);
     Treble2 = TrebleFreq << 8; 
     Treble1 = Treble1 & Treble3; // clear bits 8 to 11 
     Treble1 = Treble1 | Treble2; // set bits 8 to 11 = treble2
     vs1053SciWrite(vs1053_REG_BASS, Treble1); 
    
}
/////////////////////////////////////////////////////////////
// 20 to 150 Hz
/////////////////////////////////////////////////////////////
void ChangeBassFreq(byte UpDwn)
{    
     uint16_t Bass1, Bass2 = 0;
     uint16_t Bass3 = 0xfff0;
     if (UpDwn==0) {if (BassFreq>2)  BassFreq = BassFreq - 1; } 
     if (UpDwn==1) {if (BassFreq<15) BassFreq = BassFreq + 1; } 
     Bass1 = vs1053SciRead(vs1053_REG_BASS);
     Bass2 = BassFreq; 
     Bass1 = Bass1 & Bass3; // clear bits 0 to 3 
     Bass1 = Bass1 | Bass2; // set bits 0 to 3 = bass2
     vs1053SciWrite(vs1053_REG_BASS, Bass1);  
    
}
///////////////////////////////////////////////////////////////////////////////
// In init.c
// USEX(SCI_AICTRL1) = 1024;   // Gain = 1X if 1024 2X = 2048 0.5X = 512 etc
// USEX(SCI_AICTRL3) = (1<<2); // set linear pcm mode, joint-stereo
// sciWrite(vs1053_SCI_AICTRL1, 1024); // 4096 = 4x or +12dB
///////////////////////////////////////////////////////////////////////////////
void ChangeGain(byte UpDwn)
{    
     uint16_t Gain1 = 0;
     //int fnum = 1;
     
     if (UpDwn==0) {if (Gain>0)  Gain = Gain - 1; }
     if (UpDwn==1) {if (Gain<5)  Gain = Gain + 1; }
     Gain1 = pgm_read_word (&(GainValue[Gain]));
     vs1053SciWrite(vs1053_SCI_AICTRL1, Gain1);
     // needed because else unstable if effects changed
     //fnum = FunctionNum;
     //FunctionNum = 1;
     //ReadEffects();        
     //WriteEffects(); 
     //FunctionNum = fnum; 
}

////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////
void readBtn()
{ 
  //Check if your buttons are pressed

  if (btnNext.fallingEdge()) 
     { if (FunctionNum==1) ChangeEffect(1);
       if (FunctionNum==3) ChangeBass(1);
       if (FunctionNum==4) ChangeTreble(1);
       if (FunctionNum==9) ChangeMenu(1);
       if (Menu>1)
          { if (FunctionNum==5) ChangeBassFreq(1);
            if (FunctionNum==6) ChangeTrebleFreq(1);
            if (FunctionNum==7) ChangeGain(1);
            if (FunctionNum==8) DoEeprom(1);
          }
     }
  else
  if (btnPrev.fallingEdge())  
     { if (FunctionNum==1) ChangeEffect(0);
       if (FunctionNum==3) ChangeBass(0);
       if (FunctionNum==4) ChangeTreble(0);
       if (FunctionNum==9) ChangeMenu(0);
       if (Menu>1)
          { if (FunctionNum==5) ChangeBassFreq(0);
            if (FunctionNum==6) ChangeTrebleFreq(0);
            if (FunctionNum==7) ChangeGain(0);
            if (FunctionNum==8) DoEeprom(0);
          }
     }
     
  if (btnFunc.fallingEdge()) { delay(200) ;
                     EepromDone = 0; 
                     mPos = mPos + 1;
                     DoMenu();
                   }
  delay(200);
  DoDisplay();
     
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void ChangeMenu(byte UpDwn) // Menu Basic to Adanced 0 1 2 3 0 Down 1 Up
{
  if (UpDwn==0) { if (Menu > 0) Menu = Menu - 1; }    // no Wrap-around
  if (UpDwn==1) { if (Menu < 3) Menu = Menu + 1; }

  mNum = MNumber[Menu];  // 6, 11, 15, 7
  mPos = MNumber9[Menu]; // 4 4 8 0 fixed position of functionnum = 9
  DoMenu();
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Menu Basic = 5 options (Effects Vol Bass Reble Menu) Menu Adanced = 9 options (Basic + Effects9) Menu Edit 6 options 9-14
// All menu changes start at FunctionNum = 9, Menu3 starting at <9 will be NOK
// const static byte Menu3[6]     =    { 9,10,11,12,13,14 );
// const static byte Menu2[14]    =    { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14 ); 
// const static byte Menu1[10]    =    { 1, 2, 3, 4, 9,10,11,12,13,14 ); 
// const static byte Menu0[5]     =    { 1, 2, 3, 4, 9 };
// const static byte MNumber[4]   =    { 5, 10, 14, 6 };
// const static byte MNumber9[4]  =    { 4, 4, 8, 0 };
// byte mPos, mNum = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DoMenu() // Menu Basic to Adanced 0 1 2 2
{
  if (mPos >= mNum) mPos = 0;
  if (Menu==3) FunctionNum = Menu3[mPos];
  if (Menu==2) FunctionNum = Menu2[mPos]; 
  if (Menu==1) FunctionNum = Menu1[mPos]; 
  if (Menu==0) FunctionNum = Menu0[mPos]; 
}
///////////////////////////////////////////////////////////
// Save or Retrieve from Eeprom
///////////////////////////////////////////////////////////
void DoEeprom(byte UpDwn)
{
  if (UpDwn==0) ReadEeprom();    //Down Button Read
  if (UpDwn==1) WriteEeprom();   //Up Button Write/Save
  //if (UpDwn==2) WriteEeprom0();  //Up and Down Button Default Write
}
///////////////////////////////////////////////////////////
// Could use EEPROM.update(address, value) to save #writes
// Save as four bytes: SCI_BASS and SCI_VOL with these values
// Volume      			// 255 - 0
// Bass   			// 0 - 15
// Treble			// 0 - 15
// BassFreq 			// 20 to 150 Hz
// TrebleFreq			// 1kHz to 15kHze
// Gain not saved always = 1x   // 0.5 to 5x gain
// Menu not saved always = 2    // 0 1 2 Basic to Advanced
// Board	EEPROM Size
// Teensy 4.1	4284 bytes
// Teensy 4.0	1080 bytes
// Teensy 3.6	4096 bytes
// Teensy 3.5	4096 bytes
///////////////////////////////////////////////////////////
void WriteEeprom() 
{ 
  byte b1, b2, n, i = 0;    // keep vales in eeprom through interrupts
  int VVal, CVal = 0;

  digitalWrite (ActivityLED1, HIGH); 

  // Bytes 0, 1
  EEPROM.write(0, 0xc3);    // The magic num
  EEPROM.write(1, 0x3c);
  // Bytes 2 to 13 - six parameter words for custom effect 9

  n = 2;
  for (i=0; i<6; i++)
      { EEPROM.write(n, highByte(cp9[i]));
        EEPROM.write(n+1, lowByte(cp9[i]));
        n = n+2;
      }
 
  CVal = vs1053SciRead(vs1053_REG_BASS);
  VVal = vs1053SciRead(vs1053_REG_VOLUME); 
  // Bytes 14 to 17
  EEPROM.write(14, highByte(VVal));
  EEPROM.write(15, lowByte(VVal));
  EEPROM.write(16, highByte(CVal));
  EEPROM.write(17, lowByte(CVal));

  EepromDone = 1;
  digitalWrite (ActivityLED1, LOW); 
}
////////////////////////////////////////////////
// Must restore values to SCI registers as well
////////////////////////////////////////////////
void ReadEeprom() 
{ 
  byte b1, b2, n, i = 0;    // keep vales in eeprom through interrupts
  int VVal, CVal = 0; 

  b1 = EEPROM.read(0);

  if (b1 != 0xc3) {  EepromDone = 3; return;}  // nOk

  digitalWrite (ActivityLED1, HIGH); 
  n = 2;
  for (i=0; i<6; i++)
      { b1 = EEPROM.read(n);
        b2 = EEPROM.read(n+1);
        cp9[i] = 256*b1 + b2;
        n = n+2;
      }

  WriteEffects();
    
  b1 = EEPROM.read(14);
  b2 = EEPROM.read(15);
  Volume = 256*b1 + b2;
  vs1053SetVolume(b1,b2);

  b1 = EEPROM.read(16);
  b2 = EEPROM.read(17);
  CVal = 256*b1 + b2;
  vs1053SciWrite(vs1053_REG_BASS, CVal);
  ReadControls (CVal);
  
  EepromDone = 2;
  FunctionNum = 1; // Reset Menu 2 to position 0 else next functin will change E9
  mPos = 0;
  digitalWrite (ActivityLED1, LOW); 
}
//////////////////////////////////////////////
//
//////////////////////////////////////////////
void ReadControls(uint16_t BassVal)
{ 
  uint16_t Bass1 = 0;
  uint16_t Bass2 = 0x00f0;
  uint16_t Bass3 = 0x000f;
  uint16_t Treble1 = 0;
  uint16_t Treble2 = 0xf000;
  uint16_t Treble3 = 0x0f00;
 
  Bass1 = BassVal & Bass2;
  Bass  = Bass1 >> 4;
  BassFreq = BassVal & Bass3;
  
  Treble1 = BassVal & Treble2;
  Treble  = Treble1 >> 12;
  Treble1 = BassVal & Treble3;
  TrebleFreq = Treble1 >> 8;

}

  
