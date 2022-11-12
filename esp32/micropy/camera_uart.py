import machine
import time
from machine import Pin
from machine import UART

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

uart = machine.UART(1, 9600, rx=12, tx=13, txbuf=10, rxbuf=10)
uart.init(9600, bits=8, parity=None, stop=1)

flo = str(int(10.6554353*1000))
uart.write(flo)

flash = Pin(4, Pin.OUT)
for i in range(3):
    flash.value(1)
    time.sleep(0.5)
    flash.value(0)
    time.sleep(0.5)
    print("blinked")
    
camera_status = camera.init(1)
print(camera_status)
teste = camera.capture()
print(len(teste))
camera.deinit()

camera_status = camera.init(1)
print(camera_status)
teste = camera.capture()
print(len(teste))
camera.deinit()

print("end")

#arduino_cmd("w")
#time.sleep(1)
#arduino_cmd("s")
#time.sleep(1)
