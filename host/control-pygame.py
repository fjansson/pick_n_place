"""
" Joystick control for pick-and-place machine
" Fredrik Jansson 2016
"
"
"""

import pygame
import sys

def quit():
    pygame.quit()
    print()
    sys.exit()
    
pygame.init()
#pygame.joystick.init()

size = (320,200)
#screen = pygame.display.set_mode(size)

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
    sys.exit(1)
    
# Select one of the joysticks somehow. For now, take the first
joystick = sticks[0]
axes = joystick.get_numaxes()
buttons = joystick.get_numbuttons()



while 1:
    for i in range(axes):
        val = joystick.get_axis(i)
        print ('% 5.3f'%val, end=' ')
    for i in range(buttons):
        val = joystick.get_button(i)
        print (val, end=' ')
    print('',end='\r')
#    x = joystick.get_axis(0)
#    y = joystick.get_axis(1)
#    print (x,y)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit()
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                quit()
                
    key = pygame.key.get_pressed()
    if key[pygame.K_ESCAPE]:
        print('esc')
        
    #pygame.display.flip()
    pygame.event.pump()
    pygame.time.delay(100)
