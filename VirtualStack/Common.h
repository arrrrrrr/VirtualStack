#pragma once
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>
#include <memory>
#include <functional>
#include <array>
#include <stdexcept>
#include <bitset>
#include <cmath>

enum class OpCode {
    MOV, // 000
    CAL, // 001
    POP, // 010
    RET, // 011
    ADD, // 100
    AND, // 101
    NOT, // 110
    EQU  // 111
};

enum class AddrType {
    Value, // 00
    Register, // 01
    Stack, // 10
    Pointer // 11
};

struct Instruction {
    Instruction(OpCode opcode)
        : opcode(opcode)
    {
    }

    Instruction(uint8_t src, AddrType src_type, OpCode opcode)
        : src(src), src_type(src_type), opcode(opcode)
    {
    }

    Instruction(uint8_t src, AddrType src_type, uint8_t dst, AddrType dst_type, OpCode opcode)
        : src(src), src_type(src_type), dst(dst), dst_type(dst_type), opcode(opcode)
    {
    }

    uint8_t src;
    AddrType src_type;
    uint8_t dst;
    AddrType dst_type;
    OpCode opcode;
};

class Function {
public:
    Function(int params)
        : m_params(params) {}

    void addInstruction(Instruction i) {
        m_inst.push_back(i);
    }

    const std::vector<Instruction>& getInstructions() const {
        return m_inst;
    }

    int getParamCount() {
        return m_params;
    }

private:
    int m_params;
    std::vector<Instruction> m_inst;
};
