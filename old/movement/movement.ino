#include "headers.h"

int value;
float avg;
int threshold;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < sizeof(PWM_PINS)/sizeof(PWM_PINS[0]); i++) {
    pinMode(PWM_PINS[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(DIR_PINS)/sizeof(DIR_PINS[0]); i++) {
    pinMode(DIR_PINS[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(LINE_PINS_FWD)/sizeof(LINE_PINS_FWD[0]); i++) {
    pinMode(LINE_PINS_FWD[i], INPUT);
  }
  for (int i = 0; i < sizeof(LINE_PINS_LEFT)/sizeof(LINE_PINS_LEFT[0]); i++) {
    pinMode(LINE_PINS_LEFT[i], INPUT);
  }

  // stationaryTurn(RIGHT, DEG_90);
  // stationaryTurn(LEFT, DEG_180);
  // setDirection(FWD, FWD);
  // setSpeed(MAX, MAX);
  // delay(2500);
  // setSpeed(MIN, MIN);
  // setDirection(REV, REV);
  // setSpeed(MAX, MAX);

  while (Serial.available() == 0) {}
  setSpeed(MAX, MAX);
  setDirection(FWD, FWD);

}

void stationaryTurn(bool dir, unsigned ms) {
  setSpeed(MAX, MAX);
  if (dir) {
    setDirection(LOW, HIGH);
    delay(ms);
  }
  else {
    setDirection(HIGH, LOW);
    delay(ms);
  }
  setDirection(FWD, FWD);
  setSpeed(MIN, MIN);
}

void setSpeed(int left, int right) {
  analogWrite(PWM_PINS[0], left);
  analogWrite(PWM_PINS[1], left);
  analogWrite(PWM_PINS[2], right);
  analogWrite(PWM_PINS[3], right);
}

void setDirection(int left, int right) {
  digitalWrite(DIR_PINS[0], left);
  digitalWrite(DIR_PINS[1], left);
  digitalWrite(DIR_PINS[2], right);
  digitalWrite(DIR_PINS[3], right);
}

float average_line_value() {
  int out1 = (analogRead(LINE_PINS_FWD[0]));
  int out2 = (analogRead(LINE_PINS_FWD[1]));
  int out3 = (analogRead(LINE_PINS_FWD[2]));

  // Serial.print("Out 1: ");
  // Serial.println(out1);
  // Serial.print("Out 2: ");
  // Serial.println(out2);
  // Serial.print("Out 3: ");
  // Serial.println(out3);
  avg = ((out1 + out2 + out3) / 3);
  Serial.println(avg);
  return avg;
}

bool poll_line() {
   return average_line_value() >= 995 ? true : false; //true if line, false if not
}

void loop(){
  if (poll_line()) {
    setSpeed(MIN, MIN);
    stationaryTurn(RIGHT, DEG_90);
  }
  else {
    setSpeed(MAX, MAX);
  }
}