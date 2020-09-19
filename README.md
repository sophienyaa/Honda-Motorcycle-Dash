# Honda Motorcycle Dash

**NOTE: This is a 'Alpha' and a work in progress**

[![CircleCI](https://circleci.com/gh/circleci/circleci-docs.svg?style=shield)](https://circleci.com/gh/mickwheelz/Honda-Motorcycle-Dash)

## What is this?

This is my attempt to build an information display / dashboard replacement (or additon) for modern Honda motorcycles. It connects to the bike via its diagnostic port and reads information directly from the ECU

It also contains a GPS module and support for an ambient temperature / humidity sensor

![The Display Device](/img/dash.jpg?v=4&s=200)

## Hardware

The below hardware is used in the current design. For further information see the [hardware](https://github.com/mickwheelz/Honda-Motorcycle-Dash/wiki/Hardware) section of the wiki

* Espressif ESP32 (Node32S for development, ESP32WROOM for build)
* 2.8" ILI9341 based SPI TFT Display
* DHT22 Temperature/Humidity Sensor
* Serial GPS Module
* L9637D Monolithic bus driver with ISO 9141 interface

Eagle CAD files for the hardware are available in the `pcb` folder, and designs for 3D printing a housing are available in the `case` folder

## Software & Features

The code is written for the ESP32 in C\C++ and is designed to be built with PlatformIO

On startup the device connects to the bike's ECU and displays the following information

* Engine Temperature
* Engine RPM
* Battery Voltage
* Throttle Position %
* Intake Air Temperature C
* Neutral Switch
* Side Stand Switch
* Bike Speed

It also displays the following information from other sensors

* GPS Speed (GPS)
* Ambient Temperature (DHT22)

The below features are intended to be available in the future

* Gear Position (Based on Speed + RPM) (ECU)
* Heading (North, South, East, West, etc) (GPS)
* Ambient Humidity (DHT22)
* Smartphone Notifications (ESP32 BLE)
* Smartphone "Now Playing" (ESP32 BLE)

For further information, see the [software](https://github.com/mickwheelz/Honda-Motorcycle-Dash/wiki/Software) section of the wiki section of the wiki

## Special Thanks To

This project would not be possible without the below information and open source code

Working with Honda ECU's
* Aster94's KWP2000 Library - https://github.com/aster94/Keyword-Protocol-2000
* Gonzo's CTX-OBD Project - https://gonzos.net/projects/ctx-obd/
* PGMFI Formus - http://forum.pgmfi.org/

Arduino Libraries and Tools
* Adafruit's GFX/DHT/ILI9341/BusIO Libraries - https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
* Mikalhart's TinyGPS++ - https://github.com/mikalhart/TinyGPSPlus
* Javl's image2cpp - https://javl.github.io/image2cpp/
* Ropg's truetype2gfx - https://rop.nl/truetype2gfx/
  

## Disclaimer

This code is provided under the GNU GPLv3 and includes NO WARRANTY.

The authors accept NO LIABILITY for damages or destruction to any property or persons through the use of this software or hardware design.
