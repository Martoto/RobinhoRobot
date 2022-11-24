import machine
import socket
import time
import camera
from machine import Pin
from machine import UART

def read_camera_color(uart):
    return 0

def read_floor_color(uart):
    cor = int(arduino_cmd(0b11100000, uart))
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


def receive_pose(uart):
    print("receive_pose")
    s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    try:
        s.bind(('0.0.0.0',5070)) 
        data,addr=s.recvfrom(100)
    except Exception as e:
        print("fail1", e)
        s.close()
        return
    s.close()

    try:
        print('received:',data,'from',addr)
        data = data.decode()
        print("data:", data)
        x, y, angle = eval(data)
        print("x:", x)
        print("y:", y)
        print("z:", angle)
        
        uart.write(chr(0b00000000))
        while uart.any()==0:
            img = get_image(camera)
        uart.read(1)
        
        print("x")
        uart.write(x.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image(camera)
        uart.read(1)
        
        print("y")
        uart.write(y.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image(camera)
        uart.read(1)
        
        print("a")
        uart.write(angle.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image(camera)
        uart.read(1)
    except Exception as e:
        print("fail2", e)
        return
    
def get_image(camera):
    return 0
    camera_status = camera.init(1)
    img = 0
    if camera_status == True:
        img = camera.capture()
        #print(len(teste))
        camera.deinit()
    return img

def blink(time_delay, flash):
    for i in range(2):
        flash.value(1)
        time.sleep(time_delay)
        flash.value(0)
        time.sleep(time_delay)
        print("blinked")

def arduino_cmd(cmd, uart):
    print("send", bin(cmd))
    receive_pose(uart)
    uart.write(chr(cmd))
    time.sleep(1)
    while uart.any()==0:
        img = get_image(camera)
        if(uart.any()!=0):
            break
        receive_pose(uart)
    resp = uart.read(1)
    return resp


