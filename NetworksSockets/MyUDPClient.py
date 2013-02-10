__author__ = 'DimaTWL'

import socket
import time


class MyUDPClient(object):

    HOST, PORT = "localhost", 9999
    data = "HOLA"

    def __init__(self, inpID):
        self.ID = inpID
        self.data += str(self.ID)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.connect((self.HOST, self.PORT))

    def sendOne(self):
        try:
            self.sock.sendall(self.data + "\n")
            self.received = self.sock.recv(1024)
        except Exception:
            return False

        return self.data == str(self.received).strip()
