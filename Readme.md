# Straßenbahnanzeige

Display stuff on an old tram display.

Display stuff on an Annax 40.4321.0630 tram innenanzeige. The display is a 120x8 matrix of green LEDs. We replaced the existing controller by an esp32 that can be controlled via pixelflut over serial or WiFi.

## Hardware

We replaced the hardware controller with an esp32. The actual display is controlled by the three original [LED boards](./doku.md).

## Mounting

We mount it to the wall using these [things](https://www.tinkercad.com/things/gYthnxwHkzt-innenanzeigenhalter).
