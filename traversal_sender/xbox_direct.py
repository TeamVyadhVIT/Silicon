import serial
import pygame

DEADZONE = 0.1

ser = serial.Serial('COM9', 115200)

pygame.init()
pygame.joystick.init()

js = pygame.joystick.Joystick(0)
js.init()

while True:
    pygame.event.pump()

    x = js.get_axis(2)   # left/right
    y = js.get_axis(3)   # forward/back

    if abs(x) < DEADZONE:
        x = 0.0
    if abs(y) < DEADZONE:
        y = 0.0

    # Differential mixing
    left  = y + x
    right = y - x

    # Clamp
    if left > 1.0:
        left = 1.0
    if left < -1.0:
        left = -1.0

    if right > 1.0:
        right = 1.0
    if right < -1.0:
        right = -1.0

    # LEFT SIDE
    if left >= 0:
        left_dir = 0
    else:
        left_dir = 1

    left_pwm = int(abs(left) * 255)

    # RIGHT SIDE
    if right >= 0:
        right_dir = 0
    else:
        right_dir = 1

    right_pwm = int(abs(right) * 255)

    ser.write(bytes([left_pwm, left_dir, right_pwm, right_dir]))

    print(left_pwm, left_dir, right_pwm, right_dir)
