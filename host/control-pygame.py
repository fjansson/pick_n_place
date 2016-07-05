#!/usr/bin/python3

"""
" Joystick control for pick-and-place machine
" Fredrik Jansson 2016
" fjansson at abo point fi
"
"""

import pygame
import sys
import serial
import threading
import argparse
from time import sleep

def quit():
    setSpeed(0,0,0,0)
    pygame.quit()
    print()
    sys.exit()
    
pygame.init()
#pygame.joystick.init()

#size = (320,200)
size = (1,1)
screen = pygame.display.set_mode(size)


conf = {
    'joystick_ID'  : 0,  # which joystick to use
    'button_stop'  : 5,  # note: number printed on button is index+1
    'button_down'  : 1,
    'button_up'    : 2,
    'button_left'  : 3,
    'button_right' : 4,
    'button_pickup': 0,
    'button_motors_off': 9,
    'button_motors_on' : 10,
    'axis_x' : 0,
    'axis_y': 1,
    'axis_deadzone' : 0.01,
    'Vx_max': 2000,
    'Vy_max': 2000,
    'Vz_max': 3000,
    'Vr_max': 2000,
    'serial_port': '/dev/ttyACM0',
    'serial_speed': 115200,
    'print_comm' : False,       
    #    'serial_port': '/dev/cu.usbmodem1411',
}


#conf['axis_x'] = 2 # for Dragonrise gamepad / GameStop
#conf['axis_y'] = 3

# command line arguments, will add as needed
parser = argparse.ArgumentParser(description='Pick and place!')
parser.add_argument('--serial_port', metavar='port', type=str, required=False, help='serial port to use')
parser.add_argument('--joystick_ID', metavar='id',  type=int, required=False,  help='joystick to use, default 0')

args = parser.parse_args()

# options given on the command line override values in conf 
vargs = vars(args)
print (args)
for key in vargs.keys():
    print (key)
    if vargs[key]:
        if key in conf:
            conf[key] = vargs[key]



    #serial_port.setDTR(False)
    #serial_port.setDTR(True)

serial_port = serial.Serial(conf['serial_port'], conf['serial_speed'])
sleep(1) # sleep here, after initializeing serial connection. Otherwise the arduino gets stuck
         # theory: opening the connection resets the board (DTR line). If data is transmitted too fast
         # after this, it stays in bootloader mode.

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
joystick = sticks[conf['joystick_ID']]
axes = joystick.get_numaxes()
buttons = joystick.get_numbuttons()


def send(s):
    if conf['print_comm']:
        print('TX:' + s)
    serial_port.write((s+'\n').encode())
    
def setSpeed(x,y,z,r):
    # round to closest int here to avoid 0 becoming -1
    x,y,z,r = round(x),round(y),round(z),round(r)    
    send('V%d,%d,%d,%d'%(x,y,z,r))


# receiving serial data with a separate thread #
position = [0,0,0,0]
def handle_data(data):
    if conf['print_comm']:
        print('RX:' + data.rstrip())
    for i,d in enumerate(data.split()):
        if i >= 4:
            break
        position[i] = int(d)
        
    
# read data from the serial port. Run as a separate thread.
def read_from_port(ser):
    while True:
        data = ser.readline().decode()
        handle_data(data)

# start a thread for reading from the serial port    
RXthread = threading.Thread(target=read_from_port, args=(serial_port,), daemon=True)
# daemon=True makes this thread quit when the main thread quits
RXthread.start()

pickup_active = False

# main loop - get stick position, send speed to arduino 
while 1:
    """for i in range(axes):
        val = joystick.get_axis(i)
        print ('% 5.3f'%val, end=' ')
    for i in range(buttons):
        val = joystick.get_button(i)
        print (val, end=' ')
    print('',end='\r')
    """
#    print ('Raw stick pos', joystick.get_axis(conf['axis_x']) , joystick.get_axis(conf['axis_y']) )
    jx = joystick.get_axis(conf['axis_x'])
    jy = joystick.get_axis(conf['axis_y'])

    # deadzone - small joystick readings -> 0 to avoid slow drift when idle
    if abs(jx) < conf['axis_deadzone']:
        jx = 0
    if abs(jy) < conf['axis_deadzone']:
        jy = 0
    
    Vx  =    jx * conf['Vx_max']
    Vy  =  - jy * conf['Vy_max']  # minus here to flip the y axis 
    Vz  = (joystick.get_button(conf['button_up'])     * conf['Vz_max']
         - joystick.get_button(conf['button_down'])   * conf['Vz_max'])
    Vr =  (joystick.get_button(conf['button_left'])   * conf['Vr_max']
         - joystick.get_button(conf['button_right'])  * conf['Vr_max'])

    # set the speed
    # note: rotation and z are dependent. The connection is made here.
    # to rotate at constant height, set both Z and rotation speed to the same value
    # TODO: the axis limits interact in a stupid way
    setSpeed(Vx,Vy,Vz+Vr,Vr) 
    send('W')
    if joystick.get_button(conf['button_stop']):
        quit()
        
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            quit()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            quit()
        if event.type == pygame.JOYBUTTONDOWN:
            if event.button == conf['button_pickup']:
                pickup_active = not pickup_active
                print ('pickup_active:', pickup_active)
                if pickup_active:
                    send('P')
                else:
                    send('p')
            if event.button == conf['button_motors_off']:
                send('m')
            if event.button == conf['button_motors_on']:
                send('M')
                    
    key = pygame.key.get_pressed()
    if key[pygame.K_ESCAPE]:
        print('esc')
        
    #pygame.display.flip()
    pygame.event.pump()
    pygame.time.delay(50)
    print('% 5d % 5d % 5d % 5d'%tuple(position), end='\r')

    
