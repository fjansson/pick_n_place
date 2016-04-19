#!/usr/bin/python3

"""
" Joystick control for pick-and-place machine
" Fredrik Jansson 2016
"
"
"""

import pygame
import sys

def quit():
    setSpeed(0,0,0,0)
    pygame.quit()
    print()
    sys.exit()
    
pygame.init()
#pygame.joystick.init()

size = (320,200)
#screen = pygame.display.set_mode(size)


conf = {
    'button_stop':  5,
    'button_up': 0,
    'button_down': 1,
    'axis_x' : 0,
    'axis_y': 1,
    'Vx_max': 500,
    'Vy_max': 500,
    'Vz_max': 200,
}

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


def send(s):
    print('TX:' + s)

def setSpeed(x,y,z,r):
    # TODO round to closest int here to avoid 0 becoming -1
    send('V%d,%d,%d,%d'%(x,y,z,r))
    
while 1:
    """for i in range(axes):
        val = joystick.get_axis(i)
        print ('% 5.3f'%val, end=' ')
    for i in range(buttons):
        val = joystick.get_button(i)
        print (val, end=' ')
    print('',end='\r')
    """
    
    Vx  =  joystick.get_axis(conf['axis_x'])        * conf['Vx_max']
    Vy  =  joystick.get_axis(conf['axis_y'])        * conf['Vy_max']
    Vz  = (joystick.get_button(conf['button_up'])   * conf['Vz_max']
        - joystick.get_button(conf['button_down'])  * conf['Vz_max'])
    setSpeed(Vx,Vy,Vz,0)

    if joystick.get_button(conf['button_stop']):
        quit()
        

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            quit()
                
                                
    key = pygame.key.get_pressed()
    if key[pygame.K_ESCAPE]:
        print('esc')
        
    #pygame.display.flip()
    pygame.event.pump()
    pygame.time.delay(100)
