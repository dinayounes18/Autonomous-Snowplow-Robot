// Use these compatible PWM pins for Cytron on Due
#define M1_PWM 6
#define M1_DIR 7

#define M2_PWM 8  
#define M2_DIR 9

#define M3_PWM 10
#define M3_DIR 11

#define M4_PWM 12
#define M4_DIR 13

void setup() {
  // Motor pins
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_DIR, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_DIR, OUTPUT);
  pinMode(M3_PWM, OUTPUT);
  pinMode(M3_DIR, OUTPUT);
  pinMode(M4_PWM, OUTPUT);
  pinMode(M4_DIR, OUTPUT);
  
  analogWriteResolution(8);
  Serial.begin(9600);
  Serial.println("Cytron MD10C - All Motors Test");
}

void setMotor(int pwmPin, int dirPin, int speed) {
  if (speed > 0) {
    digitalWrite(dirPin, LOW);   // Forward
    analogWrite(pwmPin, speed);
  } else if (speed < 0) {
    digitalWrite(dirPin, HIGH);  // Reverse
    analogWrite(pwmPin, abs(speed));
  } else {
    analogWrite(pwmPin, 0);      // Stop
  }
}

void loop() {
  Serial.println("All motors forward - Speed 90");
  setMotor(M1_PWM, M1_DIR, 90);
  setMotor(M2_PWM, M2_DIR, 90);
  setMotor(M3_PWM, M3_DIR, 90);
  setMotor(M4_PWM, M4_DIR, 90);
  delay(3000);
  
  Serial.println("All motors forward - Speed 130");
  setMotor(M1_PWM, M1_DIR, 130);
  setMotor(M2_PWM, M2_DIR, 130);
  setMotor(M3_PWM, M3_DIR, 130);
  setMotor(M4_PWM, M4_DIR, 130);
  delay(3000);
  
  Serial.println("All motors forward - Speed 255");
  setMotor(M1_PWM, M1_DIR, 255);
  setMotor(M2_PWM, M2_DIR, 255);
  setMotor(M3_PWM, M3_DIR, 255);
  setMotor(M4_PWM, M4_DIR, 255);
  delay(3000);
  
  Serial.println("Stop all motors");
  setMotor(M1_PWM, M1_DIR, 0);
  setMotor(M2_PWM, M2_DIR, 0);
  setMotor(M3_PWM, M3_DIR, 0);
  setMotor(M4_PWM, M4_DIR, 0);
  delay(2000);

  Serial.println("Turn");
  setMotor(M1_PWM, M1_DIR, 255);
  setMotor(M2_PWM, M2_DIR, 255);
  setMotor(M3_PWM, M3_DIR, 0);
  setMotor(M4_PWM, M4_DIR, 0);
  delay(2000);


}