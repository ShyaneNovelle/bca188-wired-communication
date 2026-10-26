#include <Wire.h>

const int SDA_PIN = 21;
const int SCL_PIN = 22;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(2000);

  int devices = 0;

  for (int address = 0x01; address <= 0x7E; address++) {
    Wire.beginTransmission(address);

    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at 0x%02X\n", address);
      devices++;
    }
  }

  if (devices == 0) {
    Serial.println("No I2C devices found");
  }
}

void loop() {
}
