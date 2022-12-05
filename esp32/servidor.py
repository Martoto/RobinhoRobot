import socket
import time
import camera
from machine import Pin

# ------------- Servidor ----------------------- #
def servidor():
    print("----------")
    #server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #server_socket.bind(('', 5000))
    #server_socket.listen(0)
    
    Pin(4, Pin.OUT).value(0)
    
    camera_status = camera.init(0, format=camera.JPEG)
    img = camera.capture()
    camera.deinit()
    print(len(img))
    txSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    txSocket.setblocking(0)
    
    txSocket.sendto(img,("192.168.101.2",5000))
    
    txSocket.close()
    time.sleep(1)
    
    #while 1:
    #    client_socket, addr = server_socket.accept()
    #    print("----------")
    #    camera_status = camera.init(0, format=camera.JPEG)
    #    img = camera.capture()
    #    camera.deinit()
    
    #    cabecalho = "HTTP/1.0 200 OK\r\n"
    #    cabecalho += 'Content-Type: image/jpeg\r\n'
    #    cabecalho += 'Content-Length: ' + str(len(img)) + '\r\n\r\n'
    #    client_socket.sendall(cabecalho.encode())
    #    client_socket.sendall(img)
    
    #    client_socket.close()

    Pin(4, Pin.OUT).value(0)
    #server_socket.close()

while True:
    servidor()
