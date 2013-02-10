__author__ = 'DimaTWL'

import socket
import time
import random
import gc


class MyTCPServer(object):

    def __init__(self):
        gc.disable()
        self.HOST, self.PORT = "localhost", 9999
        self.mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.mySocket.bind((self.HOST, self.PORT))
        self.mySocket.listen(1000000)
        self.mySocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    def run(self):
        while True:
            conn, addr = self.mySocket.accept()
            print 'Connected by', addr
            sleepTime = random.normalvariate(0.001, 0.0001)
            #print sleepTime
            time.sleep(sleepTime)
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                conn.sendall(data)
            conn.close()

if __name__ == "__main__":
    server = MyTCPServer()
    server.run()