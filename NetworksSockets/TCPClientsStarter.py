__author__ = 'DimaTWL'

import multiprocessing
import time
import random
import csv

import MyTCPClient


def start(inpID):
    client = MyTCPClient.MyTCPClient(inpID)
    sumOfTimes = 0
    repeats = 10
    for x in xrange(repeats):
        sleepTime = random.normalvariate(0.1, 0.01)
        #print sleepTime
        time.sleep(sleepTime)
        startTime = time.time()
        success = client.sendOne()
        endTime = time.time()
        if success:
            sumOfTimes += (endTime - startTime) * 1000
    meanTime = sumOfTimes / repeats
    return meanTime


if __name__ == "__main__":
    pool = multiprocessing.Pool(processes=500)
    file = open('simple.csv', 'w')
    csvWriter = csv.writer(file)
    results = []
    for x in xrange(1, 102, 10):
        print x
        result = pool.map(start, range(x))
        csvWriter.writerow([x, (sum(result) / len(result))])
        print x
    pool.close()
    file.flush()
    file.close()

