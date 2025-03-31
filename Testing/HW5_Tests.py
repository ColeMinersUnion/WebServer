import requests

def test_pipe():
    res = requests.get("http://localhost:8000/Output.o")
    assert res.status_code == 200, "Pipe test failed"
    assert res.text == "Hello World!\n", "Pipe test failed"
    print("Pipe test passed")

if __name__ == "__main__":
    test_pipe()
    print("All tests passed")