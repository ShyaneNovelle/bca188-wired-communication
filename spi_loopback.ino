#include <SPI.h>


const int SCK_PIN  = 18;
const int MISO_PIN = 19;
const int MOSI_PIN = 23;
const int CS_PIN   = 5;

const int MONITOR_BAUD = 115200;
unsigned long lastTransfer = 0;

void setup() {
  Serial.begin(MONITOR_BAUD);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
}

void loop() {
  unsigned long now = millis();

  if (now - lastTransfer >= 1000) {
    lastTransfer = now;


    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

    digitalWrite(CS_PIN, LOW);

    uint8_t received = SPI.transfer(0xA5);

    digitalWrite(CS_PIN, HIGH);

    SPI.endTransaction();

    Serial.print("Sent 0xA5, received 0x");
    if (received < 0x10) Serial.print("0");
    Serial.println(received, HEX);
  }
}