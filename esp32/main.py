
import machine
import socket
import time
import camera
import robinho_func
from machine import Pin
from machine import UART

time.sleep(1)

flash = Pin(4, Pin.OUT)
robinho_func.blink(1.0, flash)
uart = machine.UART(1, 9600, rx=12, tx=13)
uart.init(9600, bits=8, parity=None, stop=1)
uart.read()

robinho_func.blink(0.1, flash)

for i in range (1):
  robinho_func.arduino_cmd(0b00011100, uart)


robinho_func.blink(1.0, flash)

