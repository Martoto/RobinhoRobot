import machine
import socket
import camera

camera.init(0, format=camera.JPEG)

addr = socket.getaddrinfo('192.168.245.250', 80)[0][-1]
s = socket.socket()
s.bind(addr)
s.listen(1)

print('listening on', addr)
cl, addr = s.accept()
print('client connected from', addr)

teste = cl.recv(1024).decode()
print(teste)
while teste != "oi esp":
    teste = cl.recv(1024).decode()
    print(teste)

cl.send("oi PC")
    
teste = cl.recv(1024).decode()
print(teste)
while teste != "tudo certo":
    teste = cl.recv(1024).decode()
    print(teste)
    
cl.close()
