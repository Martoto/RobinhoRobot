import machine
import socket
import time
import camera
from machine import Pin
from machine import UART

def read_camera_color(uart):
    return 0

def read_floor_color(uart, PC_server):
    cor = int(arduino_cmd(0b11100000, uart, PC_server))
    print(cor)
    if cor == 0:
        return "#FFFFFF"
    elif cor == 1:
        return "#FF0000"
    elif cor == 2:
        return "#00FF00"
    elif cor == 3:
        return "#FFFF00"
    elif cor == 4:
        return "#0000FF"
    else:
        return "#000000"
         
def receive_pose(uart, PC_server):
    print("receive_pose")
    try:
        data = PC_server.recv(1024).decode()  # receive response
        print("data:", data)
        x, y, angle = eval(data)
        print("x:", x)
        print("y:", y)
        print("z:", angle)
        
        uart.write(chr(0b00000000))
        while uart.any()==0:
            img = get_image(camera)
        uart.read()
        
        print("x")
        uart.write(x)
        while uart.any()==0:
            img = get_image(camera)
        uart.read()
        
        print("y")
        uart.write(y)
        while uart.any()==0:
            img = get_image(camera)
        uart.read()
        
        print("a")
        uart.write(angle)
        while uart.any()==0:
            img = get_image(camera)
        uart.read()
    except:
         print('no data')
    
def get_image(camera):
    camera_status = camera.init(1)
    teste = camera.capture()
    #print(len(teste))
    camera.deinit()
    return teste

def blink(time_delay, flash):
    for i in range(2):
        flash.value(1)
        time.sleep(time_delay)
        flash.value(0)
        time.sleep(time_delay)
        print("blinked")

def arduino_cmd(cmd, uart, PC_server):
    print("send", bin(cmd))
    uart.write(chr(cmd))
    while uart.any()==0:
        img = get_image(camera)
        if(uart.any()!=0):
            break
        #eceive_pose(uart, PC_server)
    resp = uart.read()
    return resp



