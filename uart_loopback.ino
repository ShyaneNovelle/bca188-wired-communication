const int TX_PIN = 17;
const int RX_PIN = 16;
const int MONITOR_BAUD = 115200;

unsigned long lastPing = 0;

void setup() {
  Serial.begin(MONITOR_BAUD);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
}

void loop() {
  unsigned long now = millis();

  if (now - lastPing >= 1000) {
    lastPing = now;
    Serial2.println("PING");
  }

  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}
