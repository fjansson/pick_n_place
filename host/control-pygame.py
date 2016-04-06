"""
" Joystick control for pick-and-place machine
" Fredrik Jansson 2016
"
"
"""

import pygame


pygame.joystick.init()

# print info about joysticks in the system 
joystick_count = pygame.joystick.get_count()
sticks = []
for i in range(joystick_count):
    joystick = pygame.joystick.Joystick(i)
    joystick.init()
    name = joystick.get_name()
    axes = joystick.get_numaxes()
    buttons = joystick.get_numbuttons()
    hats = joystick.get_numhats()
    print ("Joystick %d - %s, axes: %d, buttons: %d, hats: %d"%(i, name, axes, buttons, hats) )
    sticks.append(joystick)


if joystick_count == 0:        
    print ("No joysticks found.")

# Select one of the joysticks somehow. For now, take the first
joystick = sticks[0]
axes = joystick.get_numaxes()
buttons = joystick.get_numbuttons()
 
while 1:
    for i in range(axes):
        val = joystick.get_axis(i)
        print (val, end=' ')
    for i in range(buttons):
        val = joystick.get_button(i)
        print (val, end=' ')
    print()
#    x = joystick.get_axis(0)
#    y = joystick.get_axis(1)
#    print (x,y)
    pygame.time.delay(50)
