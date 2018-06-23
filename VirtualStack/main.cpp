#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include "BytecodeParser.h"
#include "VM.h"

bool read_bytecode_file(const char *filename, std::vector<uint8_t> &bytes) {
    std::ifstream file(filename, std::ios::binary | std::ios::in | std::ios::ate);
    
    if (!file.is_open())
        return false;

    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);
    char byte;

    while (size > 0) {
        file.read(&byte, 1);
        bytes.push_back(static_cast<uint8_t>(byte));
        size -= 1;
    }

    file.close();

    return true;
}

int main(int argc, char **argv) {
    if (argc != 1) {
        std::cout << "missing bytecode file" << std::endl;
        return 1;
    }

    std::vector<uint8_t> bytes;

    const char *fn = "C:\\Users\\mikey\\OneDrive\\Documents\\Projects\\VirtualStack\\bytecode.txt";

    if (!read_bytecode_file(fn, bytes)) {
        std::cout << "failed to read bytecode file: " << fn << std::endl;
        return 1;
    }
    
    VM &vm = VM::instance();
    uint8_t result = vm.execute(bytes);
    std::cout << "Result: " << static_cast<int>(result) << std::endl;
    return 0;
}