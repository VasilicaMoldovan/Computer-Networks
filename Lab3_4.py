import sys
import socket
import time
import threading
import signal
import datetime

PORT = 7777
HOST = None
peers = {}
malData = {}
sock = None
recvSock = None

def broadcastQuery(type):
    global HOST, PORT
    msg = None
    delay = 0

    if type == "DATE":
        msg = "DATEQUERY\0"
        delay = 10
    if type == "TIME":
        msg = "TIMEQUERY\0"
        delay = 3

    while True:
            sock.sendto(str.encode(msg), (HOST, PORT))
            printPeers()
            time.sleep(delay)

def isValidTime(data):
    try:
        time.strptime(data, '%H:%M:%S\0')
        return True
    except Exception:
        return False

def isValidDate(data):
    try:
        time.strptime(data, '%d:%m:%Y\0')
        return True
    except Exception:
        return False

def validateData(data):
    data = data.split(" ")
    if len(data) == 2:
        if data[0] == "DATE" and isValidDate(data[1]):
            return "DF"
        if data[0] == "TIME" and isValidTime(data[1]):
            return "TF"
    return False

def printPeers():
    for addr in peers.keys():
        print(str(addr) + ": " + str(peers[addr]))


if __name__ == "__main__":
    try:
        if len(sys.argv) != 2:
            exit("Command line argument count should be 1")

        HOST = sys.argv[1]
        ADDR = (HOST, PORT)

        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            recvSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            recvSock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

        except socket.error:
            print("Error creating socket")
            sys.exit()

        try:
            recvSock.bind(("", PORT))
        except socket.error:
            print("Error binding socket")
            sys.exit()


        dateThr = threading.Thread(target=broadcastQuery, args=("DATE",))
        timeThr = threading.Thread(target=broadcastQuery, args=("TIME",))
        dateThr.daemon = True
        timeThr.daemon = True
        dateThr.start()
        timeThr.start()

        while True:
            data, addr = recvSock.recvfrom(1024)
            data = str(data.decode())

            if data == "TIMEQUERY\0":
                recvSock.sendto(str("TIME " + str(time.strftime("%H:%M:%S", time.localtime())) + "\0").encode(), ADDR)
            elif data == "DATEQUERY\0":
                recvSock.sendto(str("DATE " + str(time.strftime("%d:%m:%Y", time.localtime())) + "\0").encode(), ADDR)
            else:
                form = validateData(data)
                if not form:
                    malData[addr] = data
                else:
                    if addr not in peers.keys():
                        peers[addr] = [None, None, 0]
                    if form == "TF":
                        peers[addr][0] = data.rstrip("\x00")
                    if form == "DF":
                        peers[addr][1] = data.rstrip("\x00")
                
            time.sleep(0.05)


    except KeyboardInterrupt:
        print("Ending broadcast...")





