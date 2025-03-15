# ArduinoLoRaAPRSTracker
LoRa APRS Tracker with an Arduino MKR WAN 1300 or 1310

## attention: Only for ham radio operators

This software may only be used by ham radio operators with a valid call sign.

## description

As soon as a valid position is received by the GNSS receiver it send the position to the APRS network via LoRa at 433.775 MHz. During the LoRa transmission the builtin LED is active. The transmission is repeated every 120 seconds as long as the position is valid. The position can be tracked at https://aprs.fi/

The software relies on the GGA data set of the GNSS receiver.

It may transmit the position data in plain text or the compressed format to the APRS network.

## required hardware

  * [Arduino MKR WAN 1300](https://docs.arduino.cc/hardware/mkr-wan-1300/) or [Arduino MKR WAN 1310](https://docs.arduino.cc/hardware/mkr-wan-1310/)
  * GNSS receiver with serial port and GGA data set
  * suitable antenna

## connections

You only have to connect an antenna and the serial output of your GNSS receiver to the serial input of the Arduino board (Pin D13 - RX). Maybe also supply your GNSS receiver with power.

## configure software

You have to insert your call sign in line 18.
The APRS symbol code and SSID may be changed in lne 19 and 20.

## used arduino libraries

  * 107-Arduino-NMEA-Parser https://github.com/107-systems/107-Arduino-NMEA-Parser
  * Arduino LoRa https://github.com/sandeepmistry/arduino-LoRa
