# AER201-Microcontroller Subsystem
This repository contains:
- an implementation for a user-friend LCD-keypad interface that allows an operator to input up to 42 instructions in under 1-minute
- EEPROM storage implementation; stores information for up to 6 past run logs
- machine logic code responsible for the autonomous sorting, delivering, and packing of pills (3 different types)
- arduino code for control and monitoring of mechanisms (motor driven: stepper, servo, dc) and sensors
- PIC-Arduino I2C and UART communication protocols
- much more

# Files
- "LCD User Interface.X" contains all programs that run on the PIC18F4620
- "motor_control" contains all programs related to the monitoring and control of sensors and motor mechanisms using an arduino nano
