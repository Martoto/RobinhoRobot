import socket

host = "10.0.0.100"  # as both code is running on same pc
port = 80  # socket server port number

client_socket = socket.socket()  # instantiate
client_socket.connect((host, port))  # connect to the server

while(1):
	data = input(" -> ")
	client_socket.send(data.encode())
	data = client_socket.recv(1024).decode()  # receive response
	print('msg:' + data)  # show in terminal

client_socket.close()  # close the connection
