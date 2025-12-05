#include "headers.h"

//constants
int value, avg, white, obstacle;


//calculation functions
//sets speed for left and right individually
void setSpeed(int left, int right) {
  digitalWrite(SPE_PINS[0], left);
  digitalWrite(SPE_PINS[1], left);
  digitalWrite(SPE_PINS[2], right);
  digitalWrite(SPE_PINS[3], right);
}

//sets direction for left and right individually
void setDirection(int left, int right) {
  digitalWrite(DIR_PINS[0], left);
  digitalWrite(DIR_PINS[1], left);
  digitalWrite(DIR_PINS[2], right);
  digitalWrite(DIR_PINS[3], right);
}

//averages the values of the three line sensors, takes boolean for debug printing
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

//polls line sensors to check if line
bool pollLine() {
  return getLineValue() > white + THRESHOLD ? true : false; //true if line, false if not
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
  // Serial.print("Distance1: ");
  // Serial.println(distance1);
  // Serial.print("Distance2: ");
  // Serial.println(distance2);
  // Serial.println("");
  delay(20);
  if (distance1 <= DISTANCE_THRESHOLD) {
    obstacle = 1;
    return 1;
  }
  else if (distance2 <= DISTANCE_THRESHOLD) {
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

//movement functions
//turns robot in place by moving wheels in opposite directions, takes direction, time in ms, and whether to continue moving after turn
void stationaryTurn(bool dir, unsigned ms, bool cont) {
  if (dir) {
    setDirection(LOW, HIGH);
  }
  else {
    setDirection(HIGH, LOW);
  }
  setSpeed(MAX, MAX);
  delay(ms);
  setDirection(FWD, FWD);
  cont ? setSpeed(MAX, MAX) : setSpeed(MIN, MIN);
}

//turns robot while moving forward by stopping one side and moving the other, takes direction, time in ms, and whether to continue moving after turn
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

//moves robot forward for a set time in ms
void moveForward(unsigned ms, bool cont=false) {
  setDirection(FWD, FWD);
  setSpeed(MAX, MAX);
  delay(ms);
  cont ? setSpeed(MAX, MAX) : setSpeed(MIN, MIN);
}

//moves robot forward until it cannot go any further
void fwdUntilCant() {
  setDirection(FWD, FWD);
  setSpeed(MAX, MAX);
  while (!pollLine() && !pollObstacle()) {}
  setSpeed(MIN, MIN);
}

//moves robot in reverse for a set time in ms
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

//first function to run in setup()
void startup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  initPins();

  waitForSerial();
  getThreshold();
}
//initializes pins for motor board and sensors
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
    for (int i = 0; i < sizeof(LINE_PINS_LEFT)/sizeof(LINE_PINS_LEFT[0]); i++) {
        pinMode(LINE_PINS_LEFT[i], INPUT);
    }
    pinMode(TRIG1, OUTPUT);
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO1, INPUT);
    pinMode(ECHO2, INPUT);
}


//main functions
void setup() {
  startup();
  waitForSerial();
  test_all();
}

void test_movement() {
    Serial.println("Testing movement functions");
    Serial.println("Moving forward for 3 seconds");
    moveForward(3000, false);
    delay(1000);
    Serial.println("Moving reverse for 3 seconds");
    moveReverse(3000);
    delay(1000);
    Serial.println("Turning right while moving for 2 seconds");
}

void test_sensors() {
    Serial.println("Testing sensors");
    Serial.println("Place robot over line and press any key to continue");
    waitForSerial();
    getThreshold();
    Serial.print("White threshold set to: ");
    Serial.println(white);
    Serial.println("Place robot in front of obstacle and press any key to continue");
    waitForSerial();
    int obs = pollObstacle();
    if (obs == 1) {
      Serial.println("Obstacle detected on left sensor");
    }
    else if (obs == 2) {
      Serial.println("Obstacle detected on right sensor");
    }
    else {
      Serial.println("No obstacle detected");
    }
}

void test_all() {
    test_sensors();
    test_movement();
}

void loop(){
  
}