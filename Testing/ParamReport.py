import requests
import threading
import time
import subprocess
import random

#Each Test
#Should make 100 requests, in batches of 10(?), 20(?) to the webserver
#I'm trying to find how many requests can be handled in parallel, throughput, files per time and average response.

#After each test, change num threads in WebServer.cfg. Threads should iterate from 1 to 10.
#Restart webserver.
#Once each test is run, use matplotlib to plot the data against the number of threads. 

#To run the webserver, i want to create
def run_webserver():
    #Starting the webserver
    return subprocess.Popen(['../build/WebServer'])

def end_process(process: subprocess.Popen):
    #Stopping the webserver
    process.terminate()
    time.sleep(1)

#Increments the threadpool size in WebServer.cfg
def threadpool_size():
    for i in range(1, 11):
        with open('../WebServer.cfg', 'w') as f:
            f.write(f'ROOT=/Users/chansen/WebServer/bin\nPORT=8000\nNUM_THREADS={i}\nFILE_NOT_FOUND=/404.html\nINDEX=/index.html')
        yield i


FILES = ('404.html',
         'Extend.o',
         'File.txt',
         'FileStructure.png',
         'goat.jpg',
         'index.html',
         'Hat.jpg',
         'index.html',
         'KnivesOutScript.txt',
         'MemoryLayout.png',
         'Output.exe',
         'Output.o',
         'test.txt',
         'Thread_Process.png')

#GEnerator to increment webserver threadpool size

def test_webserver(threads: int):
    pass


# As defined by boost::asio::streambuf::max_size()
# Network buffer size = 0xFFFFFFFFFFFFFFFF or 18446744073709551615

if __name__ == '__main__':
    data = []
    for i in threadpool_size():
        print(f'Running webserver with {i} threads')
        server = run_webserver()

        time.sleep(10)
        end_process(server)

