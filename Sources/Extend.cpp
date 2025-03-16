//creating a text file
//Every time the executeable is called, if the file does not exist
//It will create the file, otherwise.
//Otherwise it will append some set length of text to the file.
//Needed proof that the executeable was called and executed properly.
//I can then check that the file is now longer than it was before calling the exe

#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string file_path = "test.txt";
    std::ofstream file(file_path, std::ios::app);
    if (!file) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }
    file << "Another One. Thank you\n" << std::endl;
    file.close();
    return 0;
}