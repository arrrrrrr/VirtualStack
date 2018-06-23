#pragma once
#include "Common.h" 
#include "Instruction.h"

class BytecodeParser
{
public:
    BytecodeParser();
    ~BytecodeParser();

    std::vector<Function> parse(std::vector<uint8_t> &bytes);

private:
    OpCode decode_opcode(uint8_t bits);
    AddrType decode_addrtype(uint8_t bits);
    Instruction decode_instruction(std::vector<uint8_t> &bytes, int *curr_bit);
    uint8_t extract_bits(std::vector<uint8_t> &bytes, int at, int len);
};

