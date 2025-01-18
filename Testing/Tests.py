import requests
import random

"""
I'm using pytest as a way to organize and efficiently run tests. I do specific tests manually,
such as browser compatability. The two tests shown here are to test the deliverables of the 
assignment. The first demonstrates that the server is running and the "Hello World" is returned.
The second test demonstrates that the server handles any URL and returns the expected result.
I have not found through testing a way to create a 'client' error that the server needs to handle.
"""

def test_response():
    response = requests.get('http://localhost:8080/')
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'

def test_localhosts():
    #* Localhost
    response = requests.get('http://localhost:8080/')
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'

    #* 127.0.0.1
    response = requests.get('http://127.0.0.1:8080/')
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'

    #* 0.0.0.0
    response = requests.get('http://0.0.0.0:8080/')
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'



def test_RandomURLS():
    url = 'http://localhost:8080/'
    for _ in range(random.randint(1, 50)):
        url += random.choice('abcdefghijklmnopqrstuvwxyz1234567890')
    response = requests.get(url)
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'

