# Wiegand 26 and Wiegand 34 library for Arduino

The Wiegand interface is a de facto standard commonly used to connect a card reader or keypad to an electronic entry system. Wiegand interface has the ability to transmit signal over long distance with a simple 3 wires connection. This library uses interrupt pins from Arduino to read the pulses from Wiegand interface and return the code and type of the Wiegand.

## Requirements

The following are needed 

* [Arduino] (http://www.arduino.cc) Any ATMEGA328 compatible board should work.
* [Wiegand RFID Reader] (http://www.monkeyboard.org/products/85-developmentboard/84-rfid-wiegand-protocol-development-kit) The code was written for this reader however customers reported working with [HID] (http://www.hidglobal.com/products/cards-and-credentials) compatible readers.
* DATA0 of Wiegand connects to Arduino PIN 2 and DATA1 of Wiegand connects to Arduino PIN 3

## Installation 

Create a folder named Wiegand in Arduino's libraries folder.  You will have the following folder structure:

	cd arduino/libraries
	mkdir Wiegand
	cd Wiegand
	git clone https://github.com/monkeyboard/Wiegand-Protocol-Library-for-Arduino.git .

## Arduino Sketch

![alt text](http://www.monkeyboard.org/images/tutorials/wiegand/wiegand_arduino.png "RFID Reader to Arduino connection diagram")


Execute Arduino IDE, select Example-->Wiegand-->WiegandTest

### Example
<pre><code>
#include &lt;Wiegand.h&gt;

WIEGAND wg;

void setup() {
	Serial.begin(9600);  
	wg.begin();
}

void loop() {
	if(wg.available())
	{
		Serial.print("Wiegand HEX = ");
		Serial.print(wg.getCode(),HEX);
		Serial.print(", DECIMAL = ");
		Serial.print(wg.getCode());
		Serial.print(", Type W");
		Serial.println(wg.getWiegandType());    
	}
}
</code></pre>


## Contributors

[Francesco Uggetti (ugge75)](https://github.com/ugge75) improved this version of library to support multiple readers for ATMEGA2560.  Please check out [his version of multiple wiegand readers library here](https://github.com/ugge75/Wiegand-Protocol-Library-for-Arduino-MEGA-2560)

[Apollon77](https://github.com/Apollon77) improved interrupt safety and removed sysTick from global

[paulfurley](https://github.com/paulfurley) added 4 bit code

[PaulStoffregen](https://github.com/PaulStoffregen) added Use digitalPinToInterrupt on newer Arduino software, if present

Written by [JP Liew](http://jpliew.com)

Project home: http://www.monkeyboard.org/tutorials/82-protocol/24-wiegand-converter

*This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.*

*This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.*
