#include <Arduino.h>

#define PPR 600

volatile unsigned long counter = 0;
unsigned long c0;
unsigned long c1;
unsigned long cd;
unsigned long rpm;
volatile bool motor_dir = 0;

// PWM control
int pwm_val = 0;
int pwm_dir = 1;   // 1 = up, -1 = down
bool dir = 0;

void isr() {
  counter++;
  // motor_dir = digitalRead(3);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  noInterrupts();
  c0 = counter;
  interrupts();

  delayMicroseconds(10000);

  noInterrupts();
  c1 = counter;
  interrupts();

  cd = (c0 > c1) ? c0 - c1 : c1 - c0;

  rpm = (cd * 6000UL) / PPR;

  analogWrite(6, pwm_val);

  pwm_val += pwm_dir;

  if (pwm_val >= 255) {
    pwm_val = 255;
    pwm_dir = -1;
  }

  if (pwm_val <= 0) {
    pwm_val = 0;
    pwm_dir = 1;

  }

  digitalWrite(5, dir);

  Serial.print(" c0 = "); Serial.print(c0);
  Serial.print(" c1 = "); Serial.print(c1);
  Serial.print(" cd = "); Serial.print(cd);
  Serial.print(" pwm = "); Serial.print(pwm_val);
  Serial.print(" dir = "); Serial.print(dir);
  Serial.print(" motor dir = "); Serial.print(motor_dir);
  Serial.print(" rpm = "); Serial.print(rpm);
  Serial.println();
}