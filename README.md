# C++ WebServer
This webserver was made during Spring 2025 as course work related to Pitt ECE1170. This class was focused on operating systems, and there are many extraneous features used to create parallels to an operating system. 

## Design Requirements
To fully understand the implementation of this design, it is necessary to explore what was required during the design of this operating system. This list of requirements includes: 

### GET Requests
This webserver was only required and designed to handle http GET requests. All other http methods have not and will not be implemented. 

### Multithreaded
This webserver ought to use multiple threads to increase capacity. 

### Static Buffers
There were two buffers used. One to read in files, and another to stream information back to the client. These buffers mimicked the swap space of an operating system, and as such needed to be statically allocated. 

### Executables
The client should be able to request an executable file, that file should run and it's output should be piped back to the user. 

### Client Control Block
One of the more vestigial features of this project was a client control block which mimicked a process control block of an operating system. Unlike the operating system, the client control block is never read from during the execution of the webserver. It does stay up to date with the most current process. 

### Configuration
This webserver should also be configurable. The configuration should specify buffer sizes, number of threads used, default files for root and file not found errors and more. 

## Design Choices
To preface the explanation of this design, I find it relevant that I explain my background. I find myself working in backed and full stack development frequently. I also really enjoy this level of abstraction (OS and Server), so from the start I knew that I wanted to make this a portfolio project of mine. As such, I wanted to make a structured and clean repository that meet my standards. 

### File Structure
There are two class objects defined and used in this project: Server and Threadpool. These classes have both a header (`.hpp`) and implementation (`.cpp`) file. The code executable is derived from `Main.cpp` and uses CMake to manage dependencies. Among my folders, 4 of them are of note:
![File Structure](/bin/FileStructure.png)
The `/build` folder is made by CMake and stores the compiled WebServer executable. It is not present on Github, but will be submitted for this assignment. The `/bin` folder is the folder served by the web server. It contains executables, images, text and html files. All of which are supported by the server. The `/Sources` folder contains the source code for any and all executables in `/bin`. Lastly, `/Testing` contains several python files used for testing and generating data for the parameter report. This folder contains 5 test files, and folder which proves they pass. These files are unit tests that correspond to homework assignments. 


### Server
The server is implemented around a function that handles request. I used boost for most of my networking, including the acceptor object which listens on a specified port for requests and queues them to be handled by the threadpool. The Threadpool gets passed a pointer to the handle request function and handles that thread with the first available thread. The threadpool handles the request. To avoid race conditions and issues with multithreading, the handle request function accepts an integer representing the thread_id. For things like buffers and control blocks, there are duplicates accessed by unique thread_ids such that no two threads will be trying to access the same piece of memory. In the function that handles requests, the program first reads in the request from the client. The handler function sees if the file exists, and if it does if it is an executable. This disctinction needed to be made early as it impacts the rest of the function. I then create the body of the response. In the case of reading a file, the program reads the file in chunks as wide as the file buffer to a string where the response body is held. For an executable, it goes to a new function. This function is responsible for forking the process. The parent process, opens a pipe to the child process. The child process runs the executable, pipes the output to the parent. The parent kills the cild, and returns the output to be used as the response body. Lastly, if the file doesn't exist, or wasn't found, the server defaults to predefined file and loads the file into memory as though it was normal file. The rest of the handler function proceeds as normal. The mime type is derived from the requested file's extension. In the case of an executable, the mime type is always `text/plain`. The headers of the response are derived, usually including a error code, either 200 or 404 depending on whether the files were found or not. Once the response has been made, it is streamed to the client in chunks using the network buffer. 


## Testing!
For testing, I wanted to ensure that this was fairly robust, so I created unit tests with pytest for each of the 5 homework assignments that led to the creation of this webserver. I also included pictures that show each test case passing in the `/Testing/proof/` folder. 

## Parameter Optimization
Included in the `/Testing/` folder includes a few important files used for the parameter report. I used `ParamReport.py` to generate `results.csv`, a csv file containing total time to send 100 requests, average request time and throughput. 1000 tests were run, iterating over 1-10 threads, 10 different file buffer sizes and 10 different network buffer sizes. During each test 100 requests were made to the server, requesting a random file. These requests were done in batches equivalent to the thread count. 


### Graphing
This data was then used in `ParamGraphs.py` to generate thirty 3D scatter plots. Each thread has three plots that show the total request time, average request time and throughput as a function of the buffer sizes. These can be found in the `/plots/` folder, named after the thread count. 

### Optimization
Lastly, to find the optimal set of my three input parameters, threads, file buffer size and network buffer size, I used machine learning to find a point that would minimize average and total request times and maximizes throughput. My implementation can be found in `/Optimization.py`.  This uses a Non-dominated Sorting Generic Algorithm (NSGA2) algorithm to find the optimal point. The optimal data set was found to be [3.77619938e-01 5.00761321e-01 3.19060727e-03], or after some rounding, 4 threads, a minimal file buffer size of 5 bytes of a file buffer and network buffer around 3200 bytes. 
