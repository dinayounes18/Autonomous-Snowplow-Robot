#include "Arduino.h"

uint16_t out1, out2, out3;

void setup() {
    Serial.begin(115200);
    pinMode(A9, INPUT);
    pinMode(A10, INPUT);
    pinMode(A11, INPUT);
}

void loop() {
    out1 = analogRead(A9);
    out2 = analogRead(A10);
    out3 = analogRead(A11);

    Serial.print("Line Follower 1: ");
    Serial.println(out1);
    Serial.print("Line Follower 2: ");
    Serial.println(out2);
    Serial.print("Line Follower 3: ");
    Serial.println(out3);

    delay(1000);
}