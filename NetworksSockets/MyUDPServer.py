__author__ = 'DimaTWL'

import socket
import time
import random
import gc


class MyUDPServer(object):

    def __init__(self):
        gc.disable()
        self.HOST, self.PORT = "localhost", 9999
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.mySocket.bind((self.HOST, self.PORT))
        self.mySocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    def run(self):
        while True:
            sleepTime = random.normalvariate(0.001, 0.0001)
            #print sleepTime
            time.sleep(sleepTime)
            data, addr = self.mySocket.recvfrom(1024)
            print 'Connected by', addr
            self.mySocket.sendto(data, addr)

if __name__ == "__main__":
    server = MyUDPServer()
    server.run()