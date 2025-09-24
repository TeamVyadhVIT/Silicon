#include <Arduino.h>

struct Message_t {
  uint16_t type;
  uint16_t ID;
  uint8_t data[64];
};


Message_t msg;
uint8_t j;

void setup() {
  Serial.begin(115200);
  // UART2.begin(115200, SERIAL_8N1, 16, 17); 
  msg.type = 0x1000;
  msg.ID = 0x4321;
}

void loop() {

  // //encode as big-endian (MSB first)
  // Serial.write(msg.type >> 8); // MSB
  // Serial.write(msg.type); // LSB
  // Serial.write(msg.ID >> 8);
  // Serial.write(msg.ID);
  // Serial.write(msg.data, sizeof(msg.data));

  Serial.write((uint8_t*)&msg, sizeof(msg));

  for (uint8_t i=0;i<64;i++) {
    msg.data[i] = j;
  }
  j++;

  delay(2000);
}

