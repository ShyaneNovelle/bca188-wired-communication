# Laboratory Activity 8: Wired Communication

This lab verifies the three wired interfaces on an ESP32: UART (Serial2), I2C (Wire), and SPI. Each interface was tested with its wiring connected and then removed, and the Serial Monitor output was recorded for every case.

## Environment

The board is an ESP32 with a WROOM module, selected as ESP32-WROOM-DA Module or ESP32 Dev Module in the Arduino IDE. The IDE is Arduino IDE 2.3.10 on Windows. The Serial Monitor runs at 115200 baud unless a test says otherwise.


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


## Wiring

| From | To |
|---|---|
| GPIO17 (TX2) | GPIO16 (RX2), UART loopback jumper |
| OLED VCC | 3V3 |
| OLED GND | GND |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| GPIO13 (MOSI) | GPIO26 (MISO), SPI loopback jumper |


<img width="1536" height="2048" alt="WIRING" src="https://github.com/user-attachments/assets/3b4701c7-4f89-4795-a21d-d2f7f9c5a8f2" />


The photo shows the ESP32 with the I2C OLED display wired to SDA, SCL, 3V3, and GND.

## Serial Monitor output

### 1. UART with jumper

<img width="1267" height="736" alt="uart_loopback_wired" src="https://github.com/user-attachments/assets/bdefb686-32f7-481f-a7e2-fd82b5defe91" />


PING appears once per second.

### 2. UART without jumper

<img width="499" height="271" alt="uart_loopback_not_wired" src="https://github.com/user-attachments/assets/55ca3034-eaf9-46e1-a799-69c35269a323" />


No PING lines follow the boot messages.

### 3. I2C with module

<img width="1279" height="750" alt="i2c_scanner_wired" src="https://github.com/user-attachments/assets/5663158b-0d00-4cec-b844-199e4183f9bf" />


### 4. I2C without module

<img width="1279" height="730" alt="i2c_scanner_not_wired" src="https://github.com/user-attachments/assets/3ff62a51-af06-482d-85d2-492f29c091d0" />


### 5. SPI with jumper

<img width="628" height="334" alt="spi_loopback_wired" src="https://github.com/user-attachments/assets/6e58dea7-f1e3-4866-ae24-37e88c5ebd21" />


### 6. SPI without jumper

<img width="482" height="336" alt="spi_loopback_not_wired" src="https://github.com/user-attachments/assets/e740aba6-0dd4-45cf-ad96-0fd0ea0c1824" />


### 7. Sketch at 9600, monitor at 115200

<img width="501" height="233" alt="uart_loopback_sketch" src="https://github.com/user-attachments/assets/0f68ab25-78f4-450c-972f-aab639ecb793" />


### 8. Sketch at 9600, monitor at 9600

<img width="504" height="262" alt="uart_loopback_sketch(9600)" src="https://github.com/user-attachments/assets/6892e0f6-3d2b-43ee-b197-bf8d5073545e" />


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
