import machine
import socket
import time
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

addr = socket.getaddrinfo('10.0.0.100', 80)[0][-1]
ESP_server = socket.socket()
ESP_server.bind(addr)
ESP_server.listen(1)
blink(0.1)

print('listening on', addr)
PC_link, addr = ESP_server.accept()
PC_link.setblocking(0)
print('client connected from', addr)
blink(0.2)

while(1):
    try:
        data = PC_link.recv(1024).decode()  # receive response
        print("msg:", data)
        data = "oi"
        PC_link.send(data.encode())
    except:
         print('no data')
    blink(0.2)
    
    flo = str(int(10.6554353*1000))
    uart.write(flo)

    get_image()
    
ESP_server.close()
print("end")