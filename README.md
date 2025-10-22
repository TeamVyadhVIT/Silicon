#  Project Name: Traversal-STM32

##  Description
This project implements STM32-based traversal logic for the new traversal system.  
It is part of the **Team Vyadh** project.  
It uses six PWM channels for motor control and six GPIO pins for setting the direction of each motor driver.  
The design also enables pins to support **I²C** and **CAN** communication.

##  Future Changes
- **Nano** will be used to receive feedback from encoders.  
- This will be integrated with the **CAN bus**.  
- It will also communicate with the controller via **LoRa**.

## Hardware Used
- **STM32 Microcontroller (Blue Pill)**  
- **Motor Driver:** Cytron MD10C  

##  Software Requirements
- **STM32CubeIDE**
