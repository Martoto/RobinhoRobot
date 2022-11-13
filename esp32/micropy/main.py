import machine
import socket
import time
from machine import Pin
from machine import UART
import camera
import robinho_func

flash = Pin(4, Pin.OUT)
uart = machine.UART(1, 9600, rx = 12, tx = 13)
uart.init(9600, bits = 8, parity = None, stop = 1) 

addr = socket.getaddrinfo('10.0.0.102', 80)[0][-1]
PC_server = socket.socket()
print('connect on', addr)
PC_server.connect(addr)

if (robinho_func.read_floor_color(uart) == '#FF0000'):
  for i in range (3):
    robinho_func.arduino_cmd(0b00011100, uart)
  robinho_func.arduino_cmd(0b00011110, uart)
  robinho_func.arduino_cmd(0b00011110, uart)
else:
  robinho_func.arduino_cmd(0b00011010, uart);
  robinho_func.arduino_cmd(0b10011000, uart)
  robinho_func.arduino_cmd(0b00011011, uart);

PC_server.close()