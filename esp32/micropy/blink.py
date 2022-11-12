import time
import machine
import os
from machine import Pin
from machine import UART

uart = machine.UART(1, 9600, rx=12, tx=13)
uart.init(9600, bits=8, parity=None, stop=1)

Pin(4, Pin.OUT).value(1)
time.sleep(0.06)
Pin(4, Pin.OUT).value(0)
time.sleep(0.06)
print("end")
