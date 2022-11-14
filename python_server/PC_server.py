import socket

addr = socket.getaddrinfo('10.0.0.102', 93)[0][-1]
ESP_server = socket.socket()
ESP_server.bind(addr)
ESP_server.listen(1)

print('listening on', addr)
PC_link, addr = ESP_server.accept()
PC_link.setblocking(0)
print('client connected from', addr)

data = " "
while data != "a":
	data = input(" -> ")
	data = "x:1.23490232 y:2.89482904 z:-0.4292067"
	PC_link.send(data.encode())
	#data = client_socket.recv(1024).decode()  # receive response
	#print('msg:' + data)  # show in terminal

ESP_server.close()  # close the connection