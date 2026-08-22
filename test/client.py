import socket

s = socket.create_connection(("127.0.0.1", 6379))
s.sendall(b'hello')

print(s.recv(1024).decode())
s.close()
