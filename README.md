# VS1053B-Teensy-36-and-41-Music-Effects
VS1053 Teensy36 Teensy41 Music Effects Player

VS1053B		Teensy 
Adafruit	3.6 and 4.1 (1, 2)
=====================================================================================================================
VCC 	 	5v	 
GND	 	  GND	 
SCLK 	 	Pin 13 	 		SPI Clock, shared with SD card
MISO 	 	Pin 12 	 		Input data, from VS1053/SD card
MOSI	 	Pin 11	 		Output data, to VS1053/SD card
CS 	 	  Pin 10	 		VS1053 chip select pin (output)
RST	 	  Pin 9	 		VS1053 reset pin active low (output)
XDCS	 	Pin 8	 		VS1053 Data/command select pin (output)
SDCS 	 	Pin 4	 		Chip select line for SD card
DREQ	 	Pin 3	 		VS1053 Data request (input) (ideally an Interrupt pin)

1 and 2: Adafruit VS1053B 2020 lib and Teensy 2020 lib with VS1053playerTeensy
3 Standalone VS1053playerTeensy

For Teensy 4.1 need to comment out the Status Regsiter Save SREG else get 'SREG' was not declared in this scope compiler message

OLED		Teensy 
SD1306 SH1106	3.6 and 4.1  
============================== 
VCC 	 	3.3v	 
GND	 	  GND	 
SDA 	 	Pin 18 	 		 
SCL 	 	Pin 19 	
