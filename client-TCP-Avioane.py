#!/usr/bin/env python3

'''
AVIOANE TCP
'''
import socket, struct

# Create socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Establish connection to the server (TCP is connection-based)
sock.connect(("127.0.0.1",5555))

# Inform the user
print("CONNECTED")

# Variable we use for our lovely loop
hit = 0;

# GUESSING TIME
while hit!=1:

    n = int(input("Plane position: ")) # We take the position of the plane in a vector (easier)
    sock.send(struct.pack("!H",n))     # We encode the data - network standard, short int

    num = sock.recv(2)		       # Receive 2 bytes (short int) - says if the guess was correct or not
    hit = struct.unpack("!H",num)[0]   # Unpack the bytes (same standard) - function always returns a tuple!!!
 
    s = sock.recv(15)	# Receive a love note or smth (basically we print the message from the server, if we won or not
    s = s.decode("ascii") # Decode the love note
    print(s)		# Print the love note
