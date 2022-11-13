import socket

host = "10.0.0.100"  # as both code is running on same pc
port = 80  # socket server port number

addr = socket.getaddrinfo('10.0.0.102', 80)[0][-1]
PC_server = socket.socket()
PC_server.bind(addr)
PC_server.listen(1)

print('listening on', addr)
ESP_link, addr = PC_server.accept()
ESP_link.setblocking(0)
print('client connected from', addr)

while(1):
    try:
        data = ESP_link.recv(1024).decode()  # receive response
        print("msg:", data)
        data = "oi"
        ESP_link.send(data.encode())
    except:
         print('no data')
    
    flo = str(int(10.6554353*1000))
    
PC_server.close()
print("end")