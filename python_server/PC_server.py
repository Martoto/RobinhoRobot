import socket
from _thread import *

x, y, ang = (3, 1, 4)

def client_handler(connection):
	while True:
		data = str((x, y, ang)).encode()
		print("sending data: ", data)
		connection.sendall(data)

ESP_server = socket.socket()
ESP_server.bind(("0.0.0.0", 5070))
ESP_server.listen()
print("Server started")

while 1:
	Client, address = ESP_server.accept()
	Client.setblocking(0)
	start_new_thread(client_handler, (Client, ))
	print('client connected from', address)

ESP_server.close()  # close the connection
