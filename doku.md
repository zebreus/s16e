# LED board documentation

One LED board consists of 5 8x8 LED matrixes. Multiple boards can be daisy-chained. Our Straßenbahnanzeige has 3 board; a total resolution of 8x120 pixels.

## Parts

### CSM-88148GJ LED 8x8 Matrix

- [datasheet](https://www.datasheet.hk/view_online.php?id=1364333&file=0215\csm-88148g_4736479.pdf)
- One board has 5 8x8 LED matrixes

### TPIC6B595 shift register

- [datasheet](https://www.ti.com/lit/ds/symlink/tpic6b595.pdf)
- One per 8x8 LED Matrix

### 74HC244 Octal buffer/line driver; 3-state

- [datasheet](https://www.mouser.de/datasheet/2/916/74HC_HCT244-1319278.pdf)
- One per board

### 78MO5 5V Voltage Regulator

- [datasheet](https://cdn-reichelt.de/documents/datenblatt/A200/DATASHEET_MC78M00.pdf)

## Connector (output)

| Pin connector | Function  | Connected to                                           | Notes                                                    |
| ------------- | --------- | ------------------------------------------------------ | -------------------------------------------------------- |
| 1             | LED col 8 | each LED matrix, pin 5                                 |                                                          |
| 2             | LED col 7 | each LED matrix, pin 2                                 |                                                          |
| 3             | LED col 6 | each LED matrix, pin 7                                 |                                                          |
| 4             | LED col 5 | each LED matrix, pin 1                                 |                                                          |
| 5             | LED col 3 | each LED matrix, pin 8                                 |                                                          |
| 6             |           | 78MO5, pin 1 (Input)                                   | Power?                                                   |
| 7             |           | TPIC6B595, pin 9 (Output enable); 74HC244, pin 9 (2Y3) | Output enable, active-low; Not chained to the other side |
| 8             |           | only last TPIC6B595, pin 18 (SER OUT)                  | Not chained to the other side                            |
| 9             |           | 74HC244, pin 3 (2Y0)                                   | Not chained to the other side                            |
| 10            |           | TPIC6B595, pin 12 (RCK); 74HC244, pin 7 (2Y2)          | Register clock; Not chained to the other side            |
| 11            |           |                                                        | Not connected?                                           |
| 12            |           |                                                        | Not connected?                                           |
| 13            |           |                                                        | Not connected?                                           |
| 14            |           | All ground pins                                        | Ground                                                   |
| 15            |           |                                                        | 15,16 and 17 are connected                               |
| 16            |           |                                                        | 15,16 and 17 are connected                               |
| 17            |           |                                                        | 15,16 and 17 are connected                               |
| 18            | LED col 4 | each LED matrix, pin 1                                 |                                                          |
| 19            | LED col 2 | each LED matrix, pin 1                                 |                                                          |
| 20            | LED col 1 | each LED matrix, pin 9                                 |                                                          |
