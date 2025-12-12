#include <Arduino.h>

#define PPR 600

int count = 0;
unsigned long time = 0;
unsigned long time0 = 0;
unsigned long time1 = 0;
bool time_flag = 0;

int rpm;

int pwm_val = 0;
int pwm_dir = 1;  // 1 = up, -1 = down
unsigned long last_change = 0;

void isr() {
  // count++;
  // Serial.println(count);
  if (time_flag) {
    time0 = micros();
    time_flag = !time_flag;
  }
  else {
    time1 = micros();
    time_flag = !time_flag;
  }
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(5, OUTPUT);

  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(2), isr, RISING);
}

void loop() {

  unsigned long now = millis();

  if (now - last_change >= 5) { // adjust rate here
    last_change = now;

    pwm_val += pwm_dir;
    if (pwm_val >= 255) pwm_dir = -1;
    if (pwm_val <= 0)   pwm_dir = 1;

    analogWrite(5, pwm_val);
  }

  // copy shared timestamps atomically
  noInterrupts();
  long t0 = time0;
  long t1 = time1;
  interrupts();

  time = (unsigned long) abs((t0 - t1));

  Serial.print("pwm = ");
  Serial.print(pwm_val);

  Serial.print(" time = ");
  Serial.print((unsigned long)time);

  Serial.print(" time0 = ");
  Serial.print((unsigned long)t0);

  Serial.print(" time1 = ");
  Serial.print((unsigned long)t1);

  // RPM using ms
  rpm = 60000000.0 / (PPR * (double)time);

  Serial.print(" rpm = ");
  Serial.println(rpm);
}



