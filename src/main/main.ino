#include "headers.h"

//globals
int avg, white;

//rtos stuff
//mutex for guarding shared sensor data
SemaphoreHandle_t xSensorDataMutex;
//flag to signal line detection
volatile bool isLineDetected = false;
volatile int currentObstacle = 0; //0=none, 1=right, 2=left

//robot states
//state machine enum
enum RobotState { FORWARD, REVERSING, TURNING };

/*

JENNIFERS PART

*/

//movement functions
//set speed for left and right sides
void setSpeed(int left, int right) {
	//m1 speed
	analogWrite(SPE_PINS[0], left);
	//m2 speed
	analogWrite(SPE_PINS[1], left);
	//m3 speed
	analogWrite(SPE_PINS[2], right);
	//m4 speed
	analogWrite(SPE_PINS[3], right);
}

//set direction for left and right sides
void setDirection(int left, int right) {
	//m1 direction
	digitalWrite(DIR_PINS[0], left);
	//m2 direction
	digitalWrite(DIR_PINS[1], left);
	//m3 direction
	digitalWrite(DIR_PINS[2], right);
	//m4 direction
	digitalWrite(DIR_PINS[3], right);
}

//gets analog 10 bit line value from line sensors
int getLineValue(bool debug = false) {
	//read forward line sensors
	int out1 = (analogRead(LINE_PINS_FWD[0]));
	int out2 = (analogRead(LINE_PINS_FWD[1]));
	int out3 = (analogRead(LINE_PINS_FWD[2]));

	//calculate average analog value
	avg = (out1 + out2 + out3) / 3;

	if (debug) {
		Serial.print("Avg: ");
		Serial.println(avg);
	}
	return avg;
}

/*

DINAS PART

*/

//returns true if line otherwise false
bool pollLine() {
	//check if line reading exceeds calibrated white by threshold
	return getLineValue() > white + THRESHOLD ? true : false;
}

/*
GEORGES PART

*/

//checks ultrasonic sensors to see if theres an obstacle, returns 0 for none, 1 for right, 2 for left
int pollObstacle() {
	//start us1 ping
	digitalWrite(TRIG1, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG1, HIGH);
	//ensure pulse is long enough
	delayMicroseconds(10);
	digitalWrite(TRIG1, LOW);

	//measure echo duration with 5ms timeout
	double duration1 = pulseIn(ECHO1, HIGH, 5000);
	//calculate distance or set default if timeout
	double distance1 = (duration1 == 0) ? 999 : (duration1 * SOUND) / 2;

	//start us2 ping
	digitalWrite(TRIG2, LOW);
	delayMicroseconds(2);
	digitalWrite(TRIG2, HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG2, LOW);

	//measure echo duration with 5ms timeout
	double duration2 = pulseIn(ECHO2, HIGH, 5000);
	double distance2 = (duration2 == 0) ? 999 : (duration2 * SOUND) / 2;


	if (distance1 <= DISTANCE_THRESHOLD) {
		return 1; //obstacle on right
	}
	else if (distance2 <= DISTANCE_THRESHOLD) {
		return 2; //obstacle on left
	}
	else {
		return 0; //no obstacle
	}
}

/*

DINAS PART

*/

//gets a threshold for white floor value
void getThreshold() {
	//initialize white floor threshold
	white = getLineValue(DEBUG);
}

//turns the robot in place
void stationaryTurn(bool dir, unsigned ms, bool cont) {
	//set direction for stationary turn
	if (dir) setDirection(LOW, HIGH);
	else setDirection(HIGH, LOW);

	setSpeed(MAX, MAX);

	//wait to do soft turn
	vTaskDelay(ms / portTICK_PERIOD_MS);

	//set direction back to forward
	setDirection(FWD, FWD);
	//stop or continue after turn
	cont ? setSpeed(MAX, MAX) : setSpeed(MIN, MIN);
}

//reverses the robot for a set time
void moveReverse(unsigned ms) {
	//set direction to be reverse
	setDirection(REV, REV);
	setSpeed(MAX, MAX);
	//wait to do soft turn
	vTaskDelay(ms / portTICK_PERIOD_MS);
	//set direction back to forward and stop
	setDirection(FWD, FWD);
	setSpeed(MIN, MIN);
}

/*

GEORGES PART

*/

//initializes all pins
void initPins() {
	//set motor control pins as output
	for (int i = 0; i < 4; i++) {
		pinMode(SPE_PINS[i], OUTPUT);
		pinMode(DIR_PINS[i], OUTPUT);
	}
	//set line sensor pins as input
	for (int i = 0; i < 3; i++) {
		pinMode(LINE_PINS_FWD[i], INPUT);
		pinMode(LINE_PINS_LEFT[i], INPUT);
	}
	//set ultrasonic sensor pins
	pinMode(TRIG1, OUTPUT); pinMode(TRIG2, OUTPUT);
	pinMode(ECHO1, INPUT); pinMode(ECHO2, INPUT);
}

//starts up serial, pins, and random seed
void startup() {
	//start serial communication
	Serial.begin(115200);
	//seed random generator using floating analog pin
	randomSeed(analogRead(A7));
	initPins();
	getThreshold();
    // Enable the Watchdog Timer with a 30 second timeout (max for Due)
    watchdogEnable(WATCHDOG_TIMEOUT);
}

// --- RTOS Tasks ---
/*

KYLES PART



*/

//task for polling sensors
void taskPollSensors(void *pvParameters) {
	(void) pvParameters;

	//run task forever
	for (;;) {
        // Feed the Watchdog Timer to prevent a system reset
        watchdogReset();

		//poll sensors and get results
		bool lineResult = pollLine();
		int obstacleResult = pollObstacle();

		//acquire mutex lock to protect globals
		if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY) == pdTRUE) {
			//safely update the shared variables
			currentObstacle = obstacleResult;
			isLineDetected = lineResult;
			//release mutex lock
			xSemaphoreGive(xSensorDataMutex);
		}

		//yield to cpu, runs at least once per tick
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}

//task for robot control logic
void taskRobotLogic(void *pvParameters) {
	(void) pvParameters;

	RobotState currentState = FORWARD;
	//store reason for next turn (0=line, 1=obs right, 2=obs left)
	int lastObstacleReason = 0;

	//run task forever
	for (;;) {
		//local copies of sensor data
		bool line = false;
		int obst = 0;

		//check current robot behavior
		switch (currentState) {
			case FORWARD:
				//drive straight until hazard
				setDirection(FWD, FWD);
				setSpeed(MAX, MAX);

				//safely read sensors
				if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY) == pdTRUE) {
					line = isLineDetected;
					obst = currentObstacle;
					xSemaphoreGive(xSensorDataMutex);
				}

				//check for stop condition line or obstacle
				if (line || (obst > 0)) {
					setSpeed(MIN, MIN);
					lastObstacleReason = obst;
					//transition to next state
					currentState = REVERSING;
					Serial.println("Stop detected!");
				}

				//short delay to allow sensor task to acquire mutex
				vTaskDelay(10 / portTICK_PERIOD_MS);
				break;

			case REVERSING:
				Serial.println("Reversing...");
				//perform timed reverse maneuver (1 second)
				moveReverse(1000);
				//transition to next state
				currentState = TURNING;
				break;

			case TURNING:
				//decide which way to turn based on why stopped
				if (lastObstacleReason == 0) {
					Serial.println("Line! Turning right.");
					//random right turn for line avoidance
					stationaryTurn(RIGHT, DEG_90 + ((DEG_90 / 6) * random(0, 4)), false);
				}
				else if (lastObstacleReason == 1) {
					Serial.println("Obstacle Right. Turning left.");
					//fixed turn away from obs
					stationaryTurn(LEFT, DEG_45, false);
				}
				else if (lastObstacleReason == 2) {
					Serial.println("Obstacle Left. Turning right.");
					//fixed turn away from obs
					stationaryTurn(RIGHT, DEG_45, false);
				}

				//transition back to driving
				currentState = FORWARD;
				break;
		}

		//yield to cpu
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}
/*

JENNIFERS PART

*/

//setup function
void setup() {
	startup();
	Serial.println("Startup complete. Creating RTOS tasks.");

	//create global mutex
	xSensorDataMutex = xSemaphoreCreateMutex();

	if (xSensorDataMutex != NULL) {
		//create the high task priority 2 sensors
		xTaskCreate(taskPollSensors, "SensorTask", 512, NULL, 2, NULL);
		//create the low task priority 1 logic
		xTaskCreate(taskRobotLogic, "LogicTask", 512, NULL, 1, NULL);

		Serial.println("Tasks created. Starting Scheduler...");
		//start freertos scheduler
		vTaskStartScheduler();
	} else {
		Serial.println("Error: Could not create sensor mutex.");
	}
}

//loop function is empty, scheduler handles robot functionality
void loop() {
}
