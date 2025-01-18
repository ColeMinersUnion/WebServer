import requests


def test_response():
    response = requests.get('http://localhost:8080/')
    assert response.status_code == 200
    assert response.text == '<!DOCTYPE html><html><body><h1>Hello World</h1></body></html>'


if __name__ == '__main__':
    test_response()