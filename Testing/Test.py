import requests
#Testing for homework 4
def CheckTestLength()->int:
    with open("../bin/Test.txt") as f:
        lines = f.readlines()
    return len(lines)

def test_Exe():
    res = requests.get('http://localhost:8000/Extend.o')
    assert res.status_code == 200


def test_was_Executed():
    original_length = CheckTestLength()
    test_Exe()
    final_length = CheckTestLength()
    assert final_length == original_length + 2


    