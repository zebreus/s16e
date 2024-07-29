# LED board documentation

One LED board consists of 5 8x8 LED matrixes. Multiple boards can be daisy-chained. Our Straßenbahnanzeige has 3 board; a total resolution of 8x120 pixels.

There are five shift registers. They are connected to the LED matrixes and chained by connecting the `SER OUT` of a shift register to the `SER IN` of the next one.

Multiple LED boards can be chained together.

The input connector is on the more densly populated side of the board. All pins except for 7, 8, 9 and 10 are connected directly to the output connector. The data pins 7, 8, 9 and 10 are buffered by a 74HC244 and connected to the shift registers. The buffered outputs (except for the serial data line) are connected to the output connector. The data line is passed through all shift registers, the `SER OUT` of the fifth (last) shift register is then connected to the output.

| Pin connector | Function                                 |
| ------------- | ---------------------------------------- |
| 1             | LED col 8 (pin 5)                        |
| 2             | LED col 7 (pin 2)                        |
| 3             | LED col 6 (pin 7)                        |
| 4             | LED col 5 (pin 1)                        |
| 5             | LED col 3 (pin 8)                        |
| 6             | Power, more than 7V                      |
| 7             | TPIC6B595, `G`, Output enable active low |
| 8             | TPIC6B595, `SER IN`, Serila data input   |
| 9             | TPIC6B595, `SRCK`, Shift register clock  |
| 10            | TPIC6B595, `RCK`, Register clock         |
| 11            |                                          |
| 12            |                                          |
| 13            |                                          |
| 14            | Ground                                   |
| 15            |                                          |
| 16            |                                          |
| 17            |                                          |
| 18            | LED col 4 (pin 1)                        |
| 19            | LED col 2 (pin 1)                        |
| 20            | LED col 1 (pin 9)                        |

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

| Pin | Function | Connected to            | Bridged with  |
| --- | -------- | ----------------------- | ------------- |
| 1   | 1OE      | Ground                  | 1 4 6 8 10 19 |
| 2   | 1A0      | Input pin 9 (resistor)  | 2 17          |
| 3   | 2Y0      | Output pin 9            | 3             |
| 4   | 1A1      | Ground                  | 1 4 6 8 10 19 |
| 5   | 2Y1      | First shift reg pin 3   | 5             |
| 6   | 1A2      | Ground                  | 1 4 6 8 10 19 |
| 7   | 2Y2      | Output pin 10           | 7             |
| 8   | 1A3      | Ground                  | 1 4 6 8 10 19 |
| 9   | 2Y3      | Output pin 7            | 9             |
| 10  | GND      | Ground                  | 1 4 6 8 10 19 |
| 11  | 2A3      | Input pin 7 (resistor)  | 11            |
| 12  | 1Y3      | NC?                     |               |
| 13  | 2A2      | Input pin 10 (resistor) | 13            |
| 14  | 1Y2      | NC?                     |               |
| 15  | 2A1      | Input pin 8 (resistor)  | 15            |
| 16  | 1Y1      | NC?                     |               |
| 17  | 2A0      | Input pin 9 (resistor)  | 2 17          |
| 18  | 1Y0      | shift reg pin 13        | 18            |
| 19  | 2OE      | Ground                  | 1 4 6 8 10 19 |
| 20  | VCC      | 5V                      | 20            |

| From     | To                    | via |
| -------- | --------------------- | --- |
| Input 9  | shift reg pin 13      | 1A0 |
| Ground   | NC                    | 1A1 |
| Ground   | NC                    | 1A2 |
| Ground   | NC                    | 1A3 |
| Input 9  | Output pin 9          | 2A0 |
| Input 8  | First shift reg pin 3 | 2A1 |
| Input 10 | Output pin 10         | 2A2 |
| Input 7  | Output pin 7          | 2A3 |
|          |                       |     |
|          |                       |     |
|          |                       |     |

### 78MO5 5V Voltage Regulator

- [datasheet](https://cdn-reichelt.de/documents/datenblatt/A200/DATASHEET_MC78M00.pdf)

## Connector (output)

| Pin connector | Function  | Connected to                                             | Notes                                                    |
| ------------- | --------- | -------------------------------------------------------- | -------------------------------------------------------- |
| 1             | LED col 8 | each LED matrix, pin 5                                   |                                                          |
| 2             | LED col 7 | each LED matrix, pin 2                                   |                                                          |
| 3             | LED col 6 | each LED matrix, pin 7                                   |                                                          |
| 4             | LED col 5 | each LED matrix, pin 1                                   |                                                          |
| 5             | LED col 3 | each LED matrix, pin 8                                   |                                                          |
| 6             |           | 78MO5, pin 1 (Input)                                     | Power?                                                   |
| 7             |           | 74HC244, pin 9 (2Y3) ; TPIC6B595, pin 9 (Output enable); | Output enable, active-low; Not chained to the other side |
| 8             |           | only last TPIC6B595, pin 18 (SER OUT)                    | Not chained to the other side                            |
| 9             |           | 74HC244, pin 3 (2Y0) ;                                   | Not chained to the other side                            |
| 10            |           | 74HC244, pin 7 (2Y2) ; TPIC6B595, pin 12 (RCK);          | Register clock; Not chained to the other side            |
| 11            |           |                                                          | Not connected?                                           |
| 12            |           |                                                          | Not connected?                                           |
| 13            |           |                                                          | Not connected?                                           |
| 14            |           | All ground pins                                          | Ground                                                   |
| 15            |           |                                                          | 15,16 and 17 are connected                               |
| 16            |           |                                                          | 15,16 and 17 are connected                               |
| 17            |           |                                                          | 15,16 and 17 are connected                               |
| 18            | LED col 4 | each LED matrix, pin 1                                   |                                                          |
| 19            | LED col 2 | each LED matrix, pin 1                                   |                                                          |
| 20            | LED col 1 | each LED matrix, pin 9                                   |                                                          |

## Connector (input)

| Pin connector | Function  | Connected to           | Notes                                             |
| ------------- | --------- | ---------------------- | ------------------------------------------------- |
| 1             | LED col 8 | each LED matrix, pin 5 |                                                   |
| 2             | LED col 7 | each LED matrix, pin 2 |                                                   |
| 3             | LED col 6 | each LED matrix, pin 7 |                                                   |
| 4             | LED col 5 | each LED matrix, pin 1 |                                                   |
| 5             | LED col 3 | each LED matrix, pin 8 |                                                   |
| 6             |           | 78MO5, pin 1 (Input)   | Power?                                            |
| 7             |           | 74HC244, pin 11 (2A3)  | ???, via resistor ; Not chained to the other side |
| 8             |           | 74HC244, pin 15 (2A1)  | ???, via resistor ; Not chained to the other side |
| 9             |           | 74HC244, pin 17 (2A0)  | ???, via resistor ; Not chained to the other side |
| 10            |           | 74HC244, pin 13 (2A2)  | ???, via resistor ; Not chained to the other side |
| 11            |           |                        | Not connected?                                    |
| 12            |           |                        | Not connected?                                    |
| 13            |           |                        | Not connected?                                    |
| 14            |           | All ground pins        | Ground                                            |
| 15            |           |                        | 15,16 and 17 are connected                        |
| 16            |           |                        | 15,16 and 17 are connected                        |
| 17            |           |                        | 15,16 and 17 are connected                        |
| 18            | LED col 4 | each LED matrix, pin 1 |                                                   |
| 19            | LED col 2 | each LED matrix, pin 1 |                                                   |
| 20            | LED col 1 | each LED matrix, pin 9 |                                                   |
