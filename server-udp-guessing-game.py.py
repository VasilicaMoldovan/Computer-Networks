import socket
import struct

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


if __name__ == "__main__":
	with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
		sock.bind(('localhost', 19999))
		guessed = False
		number = 234
		players = []
		while True:
			data, addr = readInt(sock)
			if addr not in players:
				players.append(addr)
			print(data, addr)
			if data > number:
				sendCharArray(sock, "smaller", addr)
			elif data < number:
				sendCharArray(sock, "bigger", addr)
			elif data == number:
				winner = addr
				break
		print("The winner is", winner)
		for player in players:
			if player != winner:
				sendCharArray(sock, "You lost.", player)
			else:
				sendCharArray(sock, "You won!", player)