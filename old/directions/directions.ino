#include "headers.h"

int value, avg, white, obstacle;

void setSpeed(int left, int right) {
  analogWrite(SPE_PINS[0], left);
  analogWrite(SPE_PINS[1], left);
  analogWrite(SPE_PINS[2], right);
  analogWrite(SPE_PINS[3], right);
}

void setDirection(int left, int right) {
  digitalWrite(DIR_PINS[0], left);
  digitalWrite(DIR_PINS[1], left);
  digitalWrite(DIR_PINS[2], right);
  digitalWrite(DIR_PINS[3], right);
}

int getLineValue(bool debug=false) {
  int out1 = (analogRead(LINE_PINS_FWD[0]));
  int out2 = (analogRead(LINE_PINS_FWD[1]));
  int out3 = (analogRead(LINE_PINS_FWD[2]));

  avg = (out1 + out2 + out3) / 3;

  if (debug) {
    Serial.print("Out 1: ");
    Serial.println(out1);
    Serial.print("Out 2: ");
    Serial.println(out2);
    Serial.print("Out 3: ");
    Serial.println(out3);
    Serial.print("Average: ");
    Serial.println(avg);
  }

  return avg;
}

bool pollLine() {
  return getLineValue(true) > white + THRESHOLD ? true : false; 
}

int pollObstacle() {
  digitalWrite(TRIG1,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG1, LOW);
  double duration1 = pulseIn(ECHO1, HIGH);
  double distance1 = (duration1 * SOUND) / 2;
  digitalWrite(TRIG2,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG2, LOW);
  double duration2 = pulseIn(ECHO2, HIGH);
  double distance2 = (duration2 * SOUND) / 2;

  if (DEBUG) {
    Serial.print("Distance1: ");
    Serial.println(distance1);
    Serial.print("Distance2: ");
    Serial.println(distance2);
    Serial.println("");
  }
  

  delay(20);
  if ((distance1 <= DISTANCE_THRESHOLD) && distance1 != 0) {
    obstacle = 1;
    return 1;
  }
  else if (distance2 <= DISTANCE_THRESHOLD && distance2 != 0) {
    obstacle = 2;
    return 2;
  }
  else {
    obstacle = 0;
    return 0;
  }
}

void getThreshold() {
  white = getLineValue(DEBUG);
}

void stationaryTurn(bool dir, unsigned ms, bool cont) {
  if (dir) {
    setDirection(LOW, HIGH);
  }
  else {
    setDirection(HIGH, LOW);
  }
  setSpeed(255, 255);
  delay(ms);
  setDirection(FWD, FWD);
  cont ? setSpeed(255, 255) : setSpeed(MIN, MIN);
}

void movingTurn(bool dir, unsigned ms, bool cont) {
    setDirection(FWD, FWD);
    if (dir) {
      setSpeed(MAX, MIN);
      delay(ms);
    }
    else {
      setSpeed(MIN, MAX);
      delay(ms);
    }
    cont ? setSpeed(MAX, MAX) : setSpeed(MIN, MIN);
}

void stationaryMovingTurn(bool dir, unsigned ms, bool cont) {
    setSpeed(MIN, MIN);
    if (dir) {
      setDirection(LOW, HIGH);
    }
    else {
      setDirection(HIGH, LOW);
    }
    if (dir) {
      setSpeed(MAX, MIN);
      delay(ms / 2);
      setSpeed(MIN, MAX);
      delay(ms / 2);
    }
    else {
      setSpeed(MIN, MAX);
      delay(ms / 2);
      setSpeed(MAX, MIN);
      delay(ms / 2);
    }
  
}

void moveForward(unsigned ms, bool cont=false) {
  setDirection(FWD, FWD);
  setSpeed(MAX, MAX);
  delay(ms);
  cont ? setSpeed(MAX, MAX) : setSpeed(MIN, MIN);
}

void fwdUntilCant() {
  setDirection(FWD, FWD);
  setSpeed(MAX, MAX);
  while (!pollLine() && !pollObstacle()) {}
  setSpeed(MIN, MIN);
}

void moveReverse(unsigned ms) {
  setDirection(REV, REV);
  setSpeed(MAX, MAX);
  delay(ms);
  setDirection(FWD, FWD);
  setSpeed(MIN, MIN);
}

//waits for serial
void waitForSerial() {
  while (Serial.available() > 0) {
    char dummy = Serial.read(); 
  }
  while (Serial.available() == 0) {}
}

void startup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  initPins();

  getThreshold();
}
void initPins() {
    for (int i = 0; i < sizeof(SPE_PINS)/sizeof(SPE_PINS[0]); i++) {
        pinMode(SPE_PINS[i], OUTPUT);
    }
    for (int i = 0; i < sizeof(DIR_PINS)/sizeof(DIR_PINS[0]); i++) {
        pinMode(DIR_PINS[i], OUTPUT);
    }
    for (int i = 0; i < sizeof(LINE_PINS_FWD)/sizeof(LINE_PINS_FWD[0]); i++) {
        pinMode(LINE_PINS_FWD[i], INPUT);
    }
    pinMode(TRIG1, OUTPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(ECHO2, INPUT);
}


void setup() {
  startup();
  
}

void loop(){
  fwdUntilCant();
  moveReverse(750);
  if (obstacle == 0) {
    Serial.println("Line");
    stationaryTurn(RIGHT, DEG_90 + ((DEG_90 / 6) * random(0, 4)), false);
  }
  else if (obstacle == 1) {
    Serial.println("Obstacle on Right");
    stationaryTurn(LEFT, DEG_90 + ((DEG_90 / 6) * random(0, 2)), false);


  }
  else if (obstacle == 2) {
    Serial.println("Onstacle on Left");
    stationaryTurn(RIGHT, DEG_90 + ((DEG_90 / 6) * random(0, 2)), false);
  }
}