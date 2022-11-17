import machine
import socket
import time
import camera
import robinho_func
from machine import Pin
from machine import UART

flash = Pin(4, Pin.OUT)
uart = machine.UART(1, 9600, rx=12, tx=13)
uart.init(9600, bits=8, parity=None, stop=1)
uart.read()

robinho_func.blink(0.1, flash)
host = "192.168.100.253"  # as both code is running on same pc
port = 5070  # socket server port number
client_socket = socket.socket()  # instantiate
client_socket.connect((host, port))  # connect to the server

#robinho_func.blink(0.1, flash)

#for i in range (1):
#  robinho_func.arduino_cmd(0b00011100, uart, client_socket)
robinho_func.arduino_cmd(0b00011110, uart, client_socket)
#robinho_func.arduino_cmd(0b00011111, uart, client_socket)

robinho_func.blink(0.1, flash)
client_socket.close()

