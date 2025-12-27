import serial
import pygame

DEADZONE = 0.1

ser = serial.Serial('COM9', 115200)  # open serial port
print(ser.name)         # check which port was really used

pygame.init()
pygame.joystick.init()
print(pygame.joystick.get_count())

js = pygame.joystick.Joystick(0)
js.init()

while True:
    pygame.event.pump()
    axisx_float = js.get_axis(2)
    axisy_float = js.get_axis(3)

    if abs(axisx_float) < DEADZONE:
        axisx_float = 0.0

    if abs(axisy_float) < DEADZONE:
        axisy_float = 0.0

    axisx = int(((1 * axisx_float) + 1.0) * 127.5)
    axisy = int(((-1 * axisy_float) + 1.0) * 127.5)

    # axisx = (axisx // 8) * 8
    # axisy = (axisy // 8) * 8

    ser.write(bytes([axisx, axisy]))
    
    print(axisx, axisy)