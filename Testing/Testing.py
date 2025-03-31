import requests
from re import sub#I'm using regex to strip the response text better


URL = 'http://localhost:8000/'

def test_response():
    response = requests.get(URL)
    assert response.status_code == 200
    assert wstrip(response.text) == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'

def test_file(): 
    res = requests.get(URL + 'File.txt')
    assert res.status_code == 200
    assert wstrip(res.text) == 'Hello World'

def test_404():
    res = requests.get(URL + '404')
    assert res.status_code == 404
    #I didn't respond with text. 

def test_long_string():
    try:
        script = open('../bin/KnivesOutScript.txt', 'r')
    except FileNotFoundError:
        pdf_to_text()
        script = open('../bin/KnivesOutScript.txt', 'r')
    res = requests.get(URL + 'KnivesOutScript.txt')
    assert res.status_code == 200
    assert res.text == script.read()

def pdf_to_text():
    from pypdf import PdfReader
    pdf = PdfReader('../bin/KnivesOutScript.pdf')
    with open('../bin/KnivesOutScript.txt', 'w') as f:
        for p in pdf.pages:
            f.write(p.extract_text())

def test_binary():
    res = requests.get(URL + 'goat.jpg')
    assert res.status_code == 200
    with open('../bin/goat.jpg', 'rb') as f:
        assert res.content == f.read()

def test_binary_404():
    res = requests.get(URL + 'goat.jpeg')
    assert res.status_code == 404

def test_binary_again():
    res = requests.get(URL + 'Hat.jpg')
    assert res.status_code == 200
    with open('../bin/Hat.jpg', 'rb') as f:
        assert res.content == f.read()

    


def wstrip(s):
    return sub(r'[^\S ]+', '', sub(r' {2,}', '', s))

if __name__ == '__main__':
    res = requests.get(URL + 'File.txt')
    print(res.text)