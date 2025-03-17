# HW 1
For the purpose of this class, the inital code was written by GPT-4o. Some edits were made using the help of GitHub copilot. 

## Notes
Works in Firefox and Safari. I don't have chrome, and I did not download it for the purpose of this assignment. As such, the server was not tested on a chromium web browser. When tested with postman, the server acknowledges a valid get request, but postman shows an error. Does postman expect a response in application/json instead of text/html? I also include some unit tests I ran with pytest. I first tested basic functionality, making sure I get the correct status code and response. I also tested all of my localhost IPs: 127.0.0.1, localhost and 0.0.0.0. I also tested random urls to make sure that the server is able to listen to any request sent to the proper address and port. I then tried to break the server. I tried creating a url longer than the buffer of the server, but it still sent the response, the server just didn't read in the full url. I also tried to overload the server, but I had some issues when trying to send four simultaneuos requests, which all succeeded, when only 3 of the 4 should have returned the proper response. 

# HW 2
This code was written primarily by deepseek-r1. I was playing around with it the past two weeks. ChatGPT and GitHub copilot were co-authors, used for debugging. 

## Notes and other things.
There was a major refactoring this homework, I tried to future proof this code to make it easier to work with for the rest of the semester. When talking about allocating buffers at the start of the program, I created a streambuf object at the start, but it uses dynamic memory allocation so I'm not sure if it still applies. However, I felt that this was the best solution to keep everything working within the boost framework. 

## HW 4
All unit tests have passed. (All four python files in Testing/). I created Extend.o specifically such that it was easy to test that it works with code. This code is not super thread-safe, I'm working to build in more of that threading best practices as I go through. 