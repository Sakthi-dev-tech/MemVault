import socket

s = socket.create_connection(("127.0.0.1", 6379))

s.sendall(b'*1\r\n$4\r\nPING\r\n')
print(s.recv(1024).decode())

s.close()

s = socket.create_connection(("127.0.0.1", 6379))

s.sendall(b'unknown command')
print(s.recv(1024).decode())

s.close()
