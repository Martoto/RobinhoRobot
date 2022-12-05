import machine
import socket
import time
import camera
from machine import Pin
from machine import UART


HOSTPORTS = [("192.168.101.2", 5075)]

def sendinfo(pose):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    data = (str(pose) + "\n").encode()
    #print("sending data: ", data)
    for hostport in HOSTPORTS:
        sock.sendto(data, hostport)
    sock.close()


def read_camera_color(uart):
    print("read_camera_color")
    s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(30)

    try:
        s.bind(('0.0.0.0',5071))
        data,addr=s.recvfrom(100)
    except Exception as e:
        print("fail1", e)
        s.close()
        raise e
    s.close()

    try:
        print('received:',data,'from',addr)
        data = data.decode()
        print("data:", data)
        return data
    except Exception as e:
        print("fail4", e)
        raise e

GRID_SIZE_SMALL= 25
GRID_SIZE_LARGE =33
GRID_SMALL_X_REAL_START= 100
GRID_SMALL_Y_REAL_START =0
GRID_SMALL_X_GRID_END= 5
GRID_LARGE_X_REAL_START= 0
GRID_LARGE_Y_REAL_START= 100
GRID_LARGE_Y_GRID_START= 4
GRID_LARGE_Y_GRID_END =6
GRID_LARGE_X_GRID_END= 6


def to_grid(x,y):
    if y < GRID_LARGE_Y_REAL_START:
        return (
        (x - GRID_SMALL_X_REAL_START) // GRID_SIZE_SMALL,
        (y - GRID_SMALL_Y_REAL_START) // GRID_SIZE_SMALL
        )
    else:
        return (
        (x - GRID_LARGE_X_REAL_START) // GRID_SIZE_LARGE,
        GRID_LARGE_Y_GRID_START + (y - GRID_LARGE_Y_REAL_START) // GRID_SIZE_LARGE
        )


def read_floor_color(uart):
    x,y,angle = only_receive_pose()
    x,y = to_grid(x,y)
    print(x,y)
    if ((x == 5) and (y == 3)):
        return "#ffff00"
    elif ((x == 2) and (y == 3)) :
        return "#ff0000"
    elif ((x == 5) and (y == 0)) :
        return "#00ff00"
    elif ((x == 2) and (y == 0)) :
        return "#0000ff"
    else :
        return "#000000"

def only_receive_pose():
    print("receive_pose")
    s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(30)

    try:
        s.bind(('0.0.0.0',5070))
        data,addr=s.recvfrom(100)
    except Exception as e:
        print("fail1", e)
        s.close()
        raise e
    s.close()

    try:
        print('received:',data,'from',addr)
        data = data.decode()
        print("data:", data)
        x, y, angle = eval(data)
        print("x:", x)
        print("y:", y)
        print("z:", angle)
    except Exception as e:
        print("fail3", e)
        raise e
    return((x,y,angle))

def receive_pose(uart):
    x,y,angle=only_receive_pose()
    send_pose(x,y,angle, uart)

def send_pose(x, y, angle, uart):
    try:
        uart.write(b'\x00')
        while uart.any()==0:
            img = get_image()
        uart.read(1)
        
        print("x")
        uart.write(x.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image()
        uart.read(1)
        
        print("y")
        uart.write(y.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image()
        uart.read(1)
        
        print("a")
        uart.write(angle.to_bytes(1, "little"))
        while uart.any()==0:
            img = get_image()
        uart.read(1)
    except Exception as e:
        print("fail2", e)
        raise e
    
def get_image():
    try:
        camera_status = camera.init(1)
    except Exception as e:
        print("fail camera:", e)
        return
    
    img = 0
    if camera_status == True:
        img = camera.capture()
        camera.deinit()
        
        txSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
        txSocket.setblocking(0)
        txSocket.sendto(img,("192.168.101.2",5000))
        txSocket.close()

def blink(time_delay, flash):
    for i in range(2):
        flash.value(1)
        time.sleep(time_delay)
        flash.value(0)
        time.sleep(time_delay)
        print("blinked")

def arduino_cmd(cmd, uart):
    print("send", bin(cmd))
    sendinfo("sending " + str(cmd))
    sendinfo("uart: " + str(uart.any())) 
    receive_pose(uart)
    sendinfo("posed")
    sendinfo("uart: " + str(uart.any()))     
    uart.write(cmd.to_bytes(1, "little"))
    sendinfo("cmded")
    sendinfo("uart: " + str(uart.any())) 
    time.sleep(2)
    sendinfo("slept")
    sendinfo("uart: " + str(uart.any())) 
    while uart.any()==0:
        sendinfo("datar")        
        sendinfo("uart: " + str(uart.any())) 
        get_image()
        if(uart.any()!=0):
            break
        receive_pose(uart)
    sendinfo("waited")        
    sendinfo("uart: " + str(uart.any())) 
    resp = uart.read(1)
    sendinfo("read")    
    sendinfo("uart: " + str(uart.any())) 
    return resp



