# Laboratory Activity 8: Wired Communication

This lab verifies the three wired interfaces on an ESP32: UART (Serial2), I2C (Wire), and SPI. Each interface was tested with its wiring connected and then removed, and the Serial Monitor output was recorded for every case.

## Environment

The board is an ESP32 with a WROOM module, selected as ESP32-WROOM-DA Module or ESP32 Dev Module in the Arduino IDE. The IDE is Arduino IDE 2.3.10 on Windows. The Serial Monitor runs at 115200 baud unless a test says otherwise.

## Repository layout

```
lab8/
├── README.md
├── WIRING.png
├── uart_loopback/
│   └── uart_loopback.ino
├── i2c_scanner/
│   └── i2c_scanner.ino
└── spi_loopback/
    └── spi_loopback.ino
```

## Pin table

| Interface | Signal | GPIO | Sketch constant | Connection in test |
|---|---|---|---|---|
| UART (Serial2) | TX | 17 | `UART_TX` / `TX_PIN` | Jumper to GPIO16 |
| UART (Serial2) | RX | 16 | `UART_RX` / `RX_PIN` | Jumper to GPIO17 |
| I2C (Wire) | SDA | 21 | `SDA_PIN` | OLED SDA |
| I2C (Wire) | SCL | 22 | `SCL_PIN` | OLED SCL |
| SPI | SCK | 14 | `SPI_SCK` | Configured only, not jumpered |
| SPI | MOSI | 13 | `SPI_MOSI` | Jumper to GPIO26 |
| SPI | MISO | 26 | `SPI_MISO` | Jumper to GPIO13 |
| SPI | CS | 4 | `SPI_CS` | Held HIGH, not jumpered |

None of the chosen pins is a strapping pin (0, 2, 5, 12, 15), a flash pin (6 to 11), or an input-only pin (34 to 39). GPIO16 and GPIO17 are usable because the board has a WROOM module; WROVER boards reserve them for PSRAM.

## Wiring

| From | To |
|---|---|
| GPIO17 (TX2) | GPIO16 (RX2), UART loopback jumper |
| OLED VCC | 3V3 |
| OLED GND | GND |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| GPIO13 (MOSI) | GPIO26 (MISO), SPI loopback jumper |

![Labeled wiring photo](WIRING.png)

The photo shows the ESP32 with the I2C OLED display wired to SDA, SCL, 3V3, and GND.

## Serial Monitor output

### 1. UART with jumper

The jumper connects GPIO17 to GPIO16. The Serial Monitor runs at 115200 baud.

```
PING
PING
PING
```

PING appears once per second.

### 2. UART without jumper

The jumper is removed. Only the boot messages appear.

```
configsip: 0, SPIWP:0xee
clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
mode:DIO, clock div:1
load:0x3fff0030,len:4876
ho 0 tail 12 room 4
load:0x40078000,len:16532
load:0x40080400,len:3500
entry 0x400805b4
```

No PING lines follow the boot messages.

### 3. I2C with module

The OLED display is connected on SDA, SCL, 3V3, and GND.

```
Found device at 0x3C
```

### 4. I2C without module

The module is disconnected.

```
No I2C devices found
```

### 5. SPI with jumper

The jumper connects GPIO13 (MOSI) to GPIO26 (MISO).

```
Sent 0xA5, received 0xA5
Sent 0xA5, received 0xA5
Sent 0xA5, received 0xA5
```

### 6. SPI without jumper

The jumper is removed.

```
Sent 0xA5, received 0x0
Sent 0xA5, received 0x0
Sent 0xA5, received 0x0
```

### 7. Sketch at 9600, monitor at 115200

The sketch sends at 9600 baud but the monitor reads at 115200, so the bytes decode as garbage.

```
<garbled characters, no readable text>
```

### 8. Sketch at 9600, monitor at 9600

The sketch and the monitor both run at 9600 baud.

```
<one line of garbled boot text>
PING
PING
PING
```

The garbled first line is the ESP32 boot message, which the ROM always prints at 115200 baud. The PING lines are readable because the sketch and the monitor match.

## Expected versus observed

| Test | Expected | Observed |
|---|---|---|
| UART with jumper | PING once per second | PING once per second |
| UART without jumper | No PING after boot messages | No PING after boot messages |
| I2C with module | `Found device at 0x3C` | `Found device at 0x3C` |
| I2C without module | `No I2C devices found` | `No I2C devices found` |
| SPI with jumper | Received 0xA5 | Received 0xA5 |
| SPI without jumper | Received a value other than 0xA5 | Received 0x0 |
| Sketch at 9600, monitor at 115200 | Garbled characters | Garbled characters, no readable text |
| Sketch at 9600, monitor at 9600 | Readable PING lines | Readable PING lines, one garbled boot line above them |

## Answers

### 1. Why does UART need both sides to use the same baud rate, while SPI uses a clock line?

UART is asynchronous, so there is no shared clock: the receiver times its sampling from the start bit and assumes a known bit period, which only works when both ends use the same baud rate. SPI sends a clock on SCK alongside the data, so the receiver samples on each clock edge and needs no prior agreement on speed.

### 2. Why can two devices with the same address not share one I2C bus?

I2C selects devices by address: the master sends an address, and the device with that address responds. If two devices share an address, both respond to the same transaction and drive SDA at once, so their signals collide and the data is corrupted.

### 3. Why is CS set HIGH between SPI transfers?

CS is active low, so a slave responds only while CS is LOW; holding it HIGH deselects the slave so it ignores clock and data activity on the bus between transfers. The sketch marks this as the idle state ("slave deselected"), and CS goes LOW only for the duration of each transfer.
