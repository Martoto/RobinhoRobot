import machine
from machine import Pin
import time
import camera
from machine import UART
uart = machine.UART(1, 9600, rx=12, tx=13)
uart.init(9600, bits=8, parity=None, stop=1)

flash = Pin(4, Pin.OUT)
for i in range(3):
    flash.value(1)
    time.sleep(0.1)
    flash.value(0)
    time.sleep(0.1)
    print("blinked")

#frente, frent, back, direita, frente, esquerda, open, close 
uart.write('w')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('w')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('s')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('d')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('w')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('a')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('o')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)

uart.write('c')
while uart.any()==0:
    print('hold')
print('Done')
flash.value(1)
time.sleep(0.1)
flash.value(0)
time.sleep(0.1)
    
#camera.init(10)
#camera.quality(12)
#camera.framesize(9)
    
#test test test