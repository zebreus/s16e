# Straßenbahnanzeige

Display stuff on an old tram display.

Display stuff on an Annax 40.4321.0630 tram innenanzeige. The display is a 120x8 matrix of green LEDs. We replaced the existing controller by an esp32 that can be controlled via pixelflut over serial or WiFi.

## Software

You can open a TCP connection to the display on port 23. After rebooting it will show its current IP. It supports a pixelflut compatible protocol with a lot of extra commands including

```
Pixelflut commands:
  HELP: Print this help message
  SIZE: Returns the size of the visible canvas in pixel as SIZE <w> <h>
  PX <x> <y> Return the current color of a pixel as PX <x> <y> <rrggbb>
  PX <x> <y> <rrggbb(aa)>: Draw a single pixel at position (x, y) with the specified hex color code. ffffff is on; everything else is off.
Light up the 'Wagen hält' indicator:
  WHf: Activate the 'Wagen halt' indicator
  WH0: Deactivate the 'Wagen halt' indicator
Print some text:
  TXT <text>: Display text
Shorthands for PX:
  PX <x> <y> 0: Set the pixel at position (x, y) to black
  PX <x> <y> f: Set the pixel at position (x, y) to white
Binary commands (No newline required):
  PB<x><y><v>: Set the pixel at (x, y) to brightness <v>. <x>, <y> and <v> are one byte binary numbers.
  SP: Update the whole image. Send SP and then a bitstream for the whole image
Fancy commands (binary and untested)
  ROT<x><y>: Rotate the buffer by <x> and <y>. <x> and <y> are one byte binary numbers
  TI: Get the current time in millis as ascii string
  SL <until>: Pause processing all further commands, until time is reached. Until is a time in millis as ascii string.
  LUA <script>: Reads a lua script that will be executed every frame until the connection is closed. <script> is read until 512 bytes are read or until the string 'AUL' followed by a newline is send
  SOL: Remove the current lua script. Does nothing if no script is loaded.
```

The list above may be outdated, run the `HELP` command to get the current list.

### LUA Scripts

You can upload LUA scripts using the `LUA` command. Lua scripts can be up to 1024 bytes in size. Each connection can have one active script simultaneously. Running the `LUA` command again replaces the previous script. You can also use the `SOL` command to remove the current script. The active LUA script will be executed before every frame. THe global state is persisted between frames. You won't get any output if your script crashes.

Lua scripts can use the following functions to interact with the display.

- `setPixel(x,y,brightness,transparency) -> void`: Set the brightness of a pixel to a value from 0 to 255
- `getPixel(x,y) -> brightness`: Get the brightness of a pixel
- `setWagenHalt(bool) -> void`: Enable or disable the Wagen Hält indicator
- `getWagenHalt() -> bool`: Get the current state of the Wagen Hält indicator

Example: A pixel scrolling from left to right

```lua
LUA
if (x == nil) then
    x = 0
end
local prevX = x
x = (x + 1) % 120

setPixel(prevX, 2, 0, 255)
setPixel(x, 2, 255, 255)
AUL
```

### Additional restrictions

s16e can support up to 30 parallel connections. Between each frame up to 130 bytes of data are processed from each connection.

## Hardware

We replaced the hardware controller with an esp32. The actual display is controlled by the three original [LED boards](./doku.md).

## Mounting

We mount it to the wall using these [things](https://www.tinkercad.com/things/gYthnxwHkzt-innenanzeigenhalter).
