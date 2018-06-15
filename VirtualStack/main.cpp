#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

bool read_bytecode_file(const char *filename, std::vector<uint8_t> &bytes) {
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    
    if (!file.is_open())
        return false;

    char byte;

    while (!file.eof()) {
        file.read(&byte, 1);
        bytes.push_back(static_cast<uint8_t>(byte));
    }

    file.close();

    return true;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "missing bytecode file" << std::endl;
        return 1;
    }

    std::vector<uint8_t> bytes;

    if (!read_bytecode_file(argv[1], bytes)) {
        std::cout << "failed to read bytecode file: " << argv[1] << std::endl;
        return 1;
    }
    
    

    return 0;
}