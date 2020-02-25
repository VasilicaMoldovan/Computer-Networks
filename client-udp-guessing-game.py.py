import logging
import socket
import struct
import random
import time
import sys

def readInt(sock):
	data, addr = sock.recvfrom(4)
	data = struct.unpack("!i", data)[0]
	return data, addr

def sendInt(sock, toSend, whereTo):
	sock.sendto(struct.pack("!i", toSend), 
		whereTo)

def sendChar(sock, toSend, whereTo):
	sock.sendto(struct.pack("!c", toSend.encode('ascii')), 
		whereTo)

def readChar(sock):
	data, addr = sock.recvfrom(1)
	data = struct.unpack("!c", data)[0].decode('ascii')
	return data, addr

def sendIntArray(sock, toSend, whereTo):
	sendInt(sock, len(toSend), whereTo)
	for value in toSend:
		sendInt(sock, value, whereTo)

def sendCharArray(sock, toSend, whereTo):
	sendInt(sock, len(toSend), whereTo)
	for value in toSend:
		sendChar(sock, value, whereTo)

def readIntArray(sock):
	buff = []
	length, addr = readInt(sock)
	for i in range(length):
		buff.append(readInt(sock)[0])
	return buff, addr

def readCharArray(sock):
	buff = ""
	length, addr = readInt(sock)
	for i in range(length):
		buff+=readChar(sock)[0]
	return buff, addr




toWhere = ("localhost", 19999)

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	
	min = 0 
	top  = 1000
	guess = random.randint(min, top)
	response = ""
	while True:
		time.sleep(0.3)
		sendInt(sock, guess, toWhere)
		response, addr = readCharArray(sock)
		if response == "smaller":
			top = guess
		elif response == "bigger":
			min = guess
		else:
			print(response)
			break
		guess = random.randint(min, top)