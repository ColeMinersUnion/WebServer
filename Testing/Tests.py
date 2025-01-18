import requests

def test_get():
    response = requests.get('http://localhost:8080/')
    print(response.text)
    #assert response.status_code == 200

if __name__ == '__main__':
    test_get()