__author__ = 'DimaTWL'

import socket
import time


class MyTCPClient(object):

    HOST, PORT = "localhost", 9999
    data = "HOLA"

    def __init__(self, inpID):
        self.ID = inpID
        self.data += str(self.ID)

    def sendOne(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.sock.connect((self.HOST, self.PORT))
            self.sock.sendall(self.data + "\n")
            self.received = self.sock.recv(1024)
        except Exception:
            return False
        finally:
            self.sock.close()
        return self.data == str(self.received).strip()
