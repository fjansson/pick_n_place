#!/usr/bin/python3

"""
" Joystick control for pick-and-place machine
" Fredrik Jansson 2016
"
"
"""

import pygame
import sys
import serial
import threading

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
    'button_down': 1,
    'button_up': 2,
    'button_left': 3,
    'button_right': 4,
    'axis_x' : 0,
    'axis_y': 1,
    'axis_deadzone' : 0.01,
    'Vx_max': 500,
    'Vy_max': 500,
    'Vz_max': 200,
    'Vr_max': 200,
    'serial_port': '/dev/cu.usbmodem1411',
}

serial_port = serial.Serial(conf['serial_port']) # , baud, timeout=0)

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
    serial_port.write((s+'\n').encode())
    
def setSpeed(x,y,z,r):
    # TODO round to closest int here to avoid 0 becoming -1
    x,y,z,r = round(x),round(y),round(z),round(r)    
    send('V%d,%d,%d,%d'%(x,y,z,r))


def handle_data(data):
    print('RX:' + data.rstrip())

# read data from the serial port. Run as a separate thread.
def read_from_port(ser):
    while True:
        data = ser.readline().decode()
        handle_data(data)

# start a thread for reading from the serial port    
RXthread = threading.Thread(target=read_from_port, args=(serial_port,), daemon=True)
# daemon=True makes this thread quit when the main thread quits
RXthread.start()


while 1:
    """for i in range(axes):
        val = joystick.get_axis(i)
        print ('% 5.3f'%val, end=' ')
    for i in range(buttons):
        val = joystick.get_button(i)
        print (val, end=' ')
    print('',end='\r')
    """
    print ('Raw stick pos', joystick.get_axis(conf['axis_x']) , joystick.get_axis(conf['axis_y']) )
    jx = joystick.get_axis(conf['axis_x'])
    jy = joystick.get_axis(conf['axis_y'])

    if abs(jx) < conf['axis_deadzone']:
        jx = 0
    if abs(jy) < conf['axis_deadzone']:
        jy = 0
    
    Vx  =  jx * conf['Vx_max']
    Vy  =  jy * conf['Vy_max']
    Vz  = (joystick.get_button(conf['button_up'])     * conf['Vz_max']
         - joystick.get_button(conf['button_down'])   * conf['Vz_max'])
    Vr =  (joystick.get_button(conf['button_left'])   * conf['Vr_max']
         - joystick.get_button(conf['button_right'])  * conf['Vr_max'])

    # set the speed
    # note: rotation and z are dependent. The connection is made here.
    # to rotate at constant height, set both Z and rotation speed to the same value
    setSpeed(Vx,Vy,Vz+Vr,Vr) 
    send('W')
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


    
