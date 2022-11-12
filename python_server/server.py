import socket


def client_program():
	print("teste")
	port = 80  # socket server port number#
	host = "192.168.245.250"
	cl = socket.socket()  # instantiate
	cl.connect((host, port))  # connect to the server

	cl.send("oi esp")

	teste = cl.recv(1024).decode()
	print(teste)
	while teste != "oi PC":
		teste = cl.recv(1024).decode()
		print(teste)
		

	cl.send("tudo certo")


client_program()
