#include <Arduino.h>

struct Message_t {
  uint16_t type;
  uint16_t ID;
  uint8_t data[64];
};

uint8_t rxBuffer = 0;

Message_t msg;

void setup() {
  Serial.begin(115200);

  msg.type = 0x1100;
  msg.ID = 0x4321;
}

void loop() {

  // //encode as big-endian (MSB first)
  // Serial.write(msg_type >> 8);
  // Serial.write(msg_type);
  // Serial.write(msg_ID >> 8);
  // Serial.write(msg_ID);

  Serial.write((uint8_t*)&msg, sizeof(msg));

  // if (Serial.available()) Serial.print("received: ");

  while (Serial.available() > 0) {
    rxBuffer = Serial.read();

    Serial.write(rxBuffer);
  }

  delay(100);
}