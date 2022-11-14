import machine
import socket
import time
import camera
from machine import Pin
from machine import UART

def get_image():
    camera_status = camera.init(1)
    teste = camera.capture()
    print(len(teste))
    camera.deinit()

def blink(time_delay):
    for i in range(2):
        flash.value(1)
        time.sleep(time_delay)
        flash.value(0)
        time.sleep(time_delay)
        print("blinked")

def arduino_cmd(cmd):
    print("send")
    uart.write(cmd)
    while uart.any()==0:
        flash.value(1)
        time.sleep(0.1)
        flash.value(0)
        time.sleep(0.1)
    flash.value(1)
    time.sleep(0.5)
    flash.value(0)
    resp = uart.read()

flash = Pin(4, Pin.OUT)
uart = machine.UART(1, 9600, rx=12, tx=13, txbuf=10, rxbuf=10)
uart.init(9600, bits=8, parity=None, stop=1)


host = "10.0.0.102"  # as both code is running on same pc
port = 92  # socket server port number
client_socket = socket.socket()  # instantiate

blink(0.1)
client_socket.connect((host, port))  # connect to the server
client_socket.settimeout(0.1)
blink(0.2)

data = " "
while data != "a":
    print("+++++++++++++")
    try:
        data = client_socket.recv(1024).decode()  # receive response
        print("msg:", data)
#        data = "oi"
#        PC_link.send(data.encode())
    except:
         print('no data')
    blink(0.2)
    flo = str(int(10.6554353*1000))
    uart.write(flo)
    get_image()
    
client_socket.close()
print("end")
