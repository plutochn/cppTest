
import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print 'please input host addr to connect: //example:127.0.0.1:12345'

ip_port = raw_input()

ip,port = ip_port.split(':',1)

port = int(port)

print  ip,port

s.connect((ip,port))

recv_buff = []

data = s.recv(1024)
if data:
    recv_buff.append(data)
     
welcome_info = ''.join(recv_buff)
print welcome_info

while True:
    print 'input cmd id'
    cmd_id = int(raw_input())
    print 'input msg body'
    body = raw_input()
    body_size = len(body)+4

    msg_head = struct.pack('hhii',cmd_id,0,body_size,body_size-4)
    msg = msg_head + body

    s.send(msg)
s.close()
