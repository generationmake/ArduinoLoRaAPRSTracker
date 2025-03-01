# ArduinoLoRaAPRSTracker
LoRa APRS Tracker with an Arduino MKR WAN 1300 or 1310

## required hardware

  * [Arduino MKR WAN 1300](https://docs.arduino.cc/hardware/mkr-wan-1300/) or [Arduino MKR WAN 1310](https://docs.arduino.cc/hardware/mkr-wan-1310/)
  * GNSS receiver with serial port
  * suitable antenna
  
## connections

You only have to connect an antenna and the serial output of your GNSS receiver to the serial input of the arduino (Pin D13 - RX). Maybe also supply your GNSS receiver with power.

## configure software

You have to insert your callsign in line 18.

## used arduino libraries

  * 107-Arduino-NMEA-Parser https://github.com/107-systems/107-Arduino-NMEA-Parser
  * Arduino LoRa https://github.com/sandeepmistry/arduino-LoRa
