[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=20877301&assignment_repo_type=AssignmentRepo)

# Autonomous Snowplow Robot
## Team
***Group 14 - Celeste***

Dina Younes  
George Papoutsis   
Jennifer Ogidi-Gbegbaje   
Kyle Mathias  

## Introduction
This repository contains the code for running the implemenation of the **autonomous snowplow robot** project. 

## Hardware
The robot is controlled by an Arduino Due microcontroller board, connected to a Cytron FD04A Rev2.0 motor driver control board, which drives 4 DC motors. The arduino receives sensor inputs from 2 ultrasonic sensors and 1 line follower sensor.

## Software
The repository contains the source code for the complete implementation at [project-l3-g14-celeste/src/main](https://github.com/SYSC4805/project-l3-g14-celeste/tree/main/src/main). This folder has 2 files; [headers.h](https://github.com/SYSC4805/project-l3-g14-celeste/blob/main/src/main/headers.h) containing the includes, constants, and pins, and [main.ino](https://github.com/SYSC4805/project-l3-g14-celeste/blob/main/src/main/main.ino) which contains the main code for running the robot. The implementation uses a port of FreeRTOS for the arduino due; [DueFreeRTOS](https://github.com/bdmihai/DueFreeRTOS) to implement the core functionality through a real-time operating system capable of handling tasks before hard real-time deadlines. The repository also includes the unit tests for the robot at [project-l3-g14-celeste/UnitTests](https://github.com/SYSC4805/project-l3-g14-celeste/blob/main/UnitTests). This folder contains 2 files; [headers.h](https://github.com/SYSC4805/project-l3-g14-celeste/blob/main/UnitTests/headers.h), containing constants, includes, and pins, and [UnitTests.ino](https://github.com/SYSC4805/project-l3-g14-celeste/blob/main/UnitTests/UnitTests.ino) which contains the main unit tests for the robot.

## License and Copyright
Copyright (c) by the listed contributors above.  
This work was created as part of the requirements for SYSC4805 Computer Systems Design Lab at Carleton University for Prof. Abdullah Kadri and the teaching assistants. All rights reserved.
