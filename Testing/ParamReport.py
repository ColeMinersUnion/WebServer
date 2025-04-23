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
    #time.sleep(0.25)

def powers_of_two():
    #Generator to yield powers of two
    for i in range(7, 18):
        yield 2 ** i
    

#Increments the threadpool size in WebServer.cfg
def threadpool_size():
    for i in range(1, 11): #Threads
        for j in powers_of_two(): #File Buffer Size
            for k in powers_of_two(): #Network Buffer Size
                #Write to WebServer.cfg
                with open('../WebServer.cfg', 'w') as f:
                    f.write(f'ROOT=/Users/chansen/WebServer/bin\nPORT=8000\nNUM_THREADS={i}\nFILE_NOT_FOUND=/404.html\nINDEX=/index.html\nFILE_BUFFER_SIZE={j}\nNETWORK_BUFFER_SIZE={k}')
                yield i, j, k
    


FILES = ('404.html',
         'Extend.o',
         'File.txt',
         'FileStructure.png',
         'goat.jpg',
         'index.html',
         'index.html',
         'KnivesOutScript.txt',
         'MemoryLayout.png',
         'Output.exe',
         'Output.o',
         'test.txt',
         'Thread_Process.png')


#Takes number of threads to use as batch size for requests.
def test_webserver(num_threads: int):


    request_times = []
    request_sizes = []


    def make_request(url):
        start = time.time()
        response = requests.get(url)
        end = time.time()
        #print(f'URL: {url}, Status Code: {response.status_code}, Time Taken: {end - start:.4f} seconds')
        request_sizes.append(len(response.content))
        #Add to data list
        request_times.append((end - start))

    for _ in range(0, 100, num_threads):
        for i in range(num_threads):
            url = f'http://localhost:8000/{random.choice(FILES)}'
            t = threading.Thread(target=make_request, args=(url,))
            t.start()
            t.join()
        #time.sleep(5)

    for i in range(100 % num_threads):
        url = f'http://localhost:8000/{random.choice(FILES)}'
        thread = threading.Thread(target=make_request, args=(url,))
        thread.start()
        thread.join()
    
    return sum(request_times), sum(request_times)/len(request_times), sum(request_sizes)/sum(request_times)
            

    #Returns time taken to complete all requests, average time to complete a request, throughput


# As defined by boost::asio::streambuf::max_size()
# Network buffer size = 0xFFFFFFFFFFFFFFFF or 18446744073709551615


if __name__ == '__main__':
    #data = []
    for i, j, k in threadpool_size():
        #print(f'Running webserver with {i} threads, {j} file buffer size, {k} network buffer size')
        server = run_webserver()
        try:
            total_time, avg_time, throughput = test_webserver(i)
            print(f'Total Time: {total_time:.4f} seconds, Average Time: {avg_time:.4f} seconds, Throughput: {throughput:.4f} bytes/second')
            with open('./results.csv', 'a') as f:
                f.write(f'{i},{j},{k},{total_time},{avg_time},{throughput}\n')
        except requests.exceptions.RequestException as e:
            with open('./results.csv', 'a') as f:
                f.write(f'Process terminated: {e}\n')
        end_process(server)
        time.sleep(0.1)

