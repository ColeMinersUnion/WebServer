import threading
import time
import requests
from random import choice

URL = 'http://localhost:8000/'
endpoints = ['File.txt', 'KnivesOutScript.txt', 'goat.jpg', 'Hat.jpg', '']

def send_request(url, i, results):
    start_time = time.time()
    try:
        response = requests.get(url)
        response.raise_for_status()
        end_time = time.time()
        results[i] = end_time - start_time
        print(f"Request {i} successful, time taken: {results[i]:.4f} seconds")
        #assert response.status_code == 200
    except requests.exceptions.RequestException as e:
        print(f"Request {i} failed: {e}")
        results[i] = float('inf')

def test_multithreading(url=URL, num_requests=3):
    threads = []
    results = {}

    for i in range(num_requests):
        thread = threading.Thread(target=send_request, args=(url + choice(endpoints), i, results))
        threads.append(thread)
        thread.start()
    
    for thread in threads:
        thread.join()
    
    avg_time = sum(results.values())/len(results)
    print(f"Average response time: {avg_time:.4f} seconds")
    return results

# test something else?


if __name__ == '__main__':
    test_multithreading()