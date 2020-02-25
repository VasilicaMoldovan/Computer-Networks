#!/usr/bin/env python3

import socket, struct

'''
Create socket
'''
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

message = input("Send something to the server: ")

n = len(message)

'''
Send the length of the string, then the actual string
'''
sock.sendto(struct.pack("!H",n),("127.0.0.1", 1234))
sock.sendto(message.encode(),("127.0.0.1", 1234))

'''
Receive the counter back (recvfrom() returns a tuple)
Unpack the counter (struct.unpack() returns a tuple)
'''
buff,addr=sock.recvfrom(2)
count = struct.unpack("!H",buff)[0]


'''
Print the little shit
'''
print(count)

