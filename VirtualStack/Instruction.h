#pragma once
#include "Common.h"

class Instruction
{
public:
    Instruction(OpCode opcode)
        : dst(0), dst_type(), src(0), src_type(), opcode(opcode)
    {
    }

    Instruction(uint8_t dst, AddrType dst_type, OpCode opcode)
        : dst(dst), dst_type(dst_type), src(0), src_type(), opcode(opcode)
    {
    }

    Instruction(uint8_t dst, AddrType dst_type, uint8_t src, AddrType src_type, OpCode opcode)
        : dst(dst), dst_type(dst_type), src(src), src_type(src_type), opcode(opcode)
    {
    }
    
    ~Instruction() {};

    uint32_t encode() const;

public:
    uint8_t dst;
    AddrType dst_type;
    uint8_t src;
    AddrType src_type;
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
