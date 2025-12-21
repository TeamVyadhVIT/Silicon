#include <Arduino.h>
#include <Servo.h>

Servo ser1;

void setup() {
  ser1.attach(2);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String incomingByte = Serial.readString();
    int angle = incomingByte.toInt();
    Serial.print("angle: "); Serial.println(angle);

    ser1.write(angle);
  }
}