#include "BytecodeParser.h"

BytecodeParser::BytecodeParser()
{
}

BytecodeParser::~BytecodeParser()
{
}

std::vector<Function> BytecodeParser::parse(std::vector<uint8_t>& bytes)
{
    int total_bits = static_cast<int>(bytes.size()) * 8;
    int curr_bit = total_bits;

    std::vector<Function> functions;

    // parse backwards because we dont know how much padding is at the start
    while (curr_bit > 0) {
        // get number of instructions
        curr_bit -= 8;
        int num_inst = extract_bits(bytes, curr_bit, 8);

        std::vector<Instruction> inst;
        // decode the byte instructions
        while (num_inst > 0) {
            inst.push_back(decode_instruction(bytes, &curr_bit));
            --num_inst;
        }
        // decode the function signature
        curr_bit -= 4;
        int num_args = extract_bits(bytes, curr_bit, 4);
        curr_bit -= 4;
        int func_id = extract_bits(bytes, curr_bit, 4);

        // create the function
        Function f(num_args);
        std::reverse(inst.begin(), inst.end());
        // add the instructions in reverse
        for (size_t i = 0; i < inst.size(); ++i) {
            f.addInstruction(inst[i]);
        }

        // add the function to the function list
        functions.push_back(f);

        // can ignore the padding
        if (func_id == 0)
            break;
    }

    // reverse the function order
    std::reverse(functions.begin(), functions.end());
    return functions;
}

OpCode BytecodeParser::decode_opcode(uint8_t bits)
{
    return static_cast<OpCode>(bits);
}

AddrType BytecodeParser::decode_addrtype(uint8_t bits)
{
    return static_cast<AddrType>(bits);
}

Instruction BytecodeParser::decode_instruction(std::vector<uint8_t>& bytes, int * curr_bit)
{
    *curr_bit -= 3;
    OpCode op = decode_opcode(extract_bits(bytes, *curr_bit, 3));
    AddrType dst_type, src_type;
    uint8_t dst_addr, src_addr;

    switch (op) {
        case OpCode::MOV:
            *curr_bit -= 2;
            dst_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            dst_addr = 0;
            // this can only be register or stack
            switch (dst_type) {
                case AddrType::Value:
                    *curr_bit -= 8;
                    dst_addr = extract_bits(bytes, *curr_bit, 8);
                    break;
                case AddrType::Register:
                    *curr_bit -= 3;
                    dst_addr = extract_bits(bytes, *curr_bit, 3);
                    break;
                case AddrType::Pointer:
                case AddrType::Stack:
                    *curr_bit -= 7;
                    dst_addr = extract_bits(bytes, *curr_bit, 7);
                    break;
            }
            *curr_bit -= 2;
            src_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            src_addr = 0;
            // this can be any type
            switch (src_type) {
                case AddrType::Value:
                    *curr_bit -= 8;
                    src_addr = extract_bits(bytes, *curr_bit, 8);
                    break;
                case AddrType::Register:
                    *curr_bit -= 3;
                    src_addr = extract_bits(bytes, *curr_bit, 3);
                    break;
                case AddrType::Pointer:
                case AddrType::Stack:
                    *curr_bit -= 7;
                    src_addr = extract_bits(bytes, *curr_bit, 7);
                    break;
            }
            // return the decoded instruction
            return Instruction(dst_addr, dst_type, src_addr, src_type, op);
        case OpCode::CAL:
            *curr_bit -= 2;
            dst_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            *curr_bit -= 8;
            dst_addr = extract_bits(bytes, *curr_bit, 8);
            *curr_bit -= 2;
            src_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            *curr_bit -= 7;
            src_addr = extract_bits(bytes, *curr_bit, 7);
            // return the decoded instruction
            return Instruction(dst_addr, dst_type, src_addr, src_type, op);
        case OpCode::POP:
            *curr_bit -= 2;
            dst_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            // we know its a stack address, so its 7 bits
            *curr_bit -= 7;
            dst_addr = extract_bits(bytes, *curr_bit, 7);
            // return the decoded instruction
            return Instruction(dst_addr, dst_type, op);
        case OpCode::RET:
            // return the decoded instruction
            return Instruction(op);
        case OpCode::ADD:
        case OpCode::AND:
            // we know its a register address
            *curr_bit -= 2;
            dst_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            // extract the register address
            *curr_bit -= 3;
            dst_addr = extract_bits(bytes, *curr_bit, 3);
            // we know its a register address
            *curr_bit -= 2;
            src_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            // extract the register address
            *curr_bit -= 3;
            src_addr = extract_bits(bytes, *curr_bit, 3);
            // return the decoded instruction
            return Instruction(dst_addr, dst_type, src_addr, src_type, op);
        case OpCode::NOT:
        case OpCode::EQU:
            // we know its a register address
            *curr_bit -= 2;
            dst_type = decode_addrtype(extract_bits(bytes, *curr_bit, 2));
            // extract the register address
            *curr_bit -= 3;
            // build the instruction
            dst_addr = extract_bits(bytes, *curr_bit, 3);
            // return the decoded instruction
            return Instruction(dst_addr, dst_type, op);
        default:
            throw std::runtime_error("Bad op code");
    }
}

uint8_t BytecodeParser::extract_bits(std::vector<uint8_t>& bytes, int at, int len)
{
    uint16_t tmp = 0;
    int start_byte = at / 8;
    int new_at = at % 8;

    if (static_cast<size_t>(start_byte) != bytes.size() - 1)
        tmp |= bytes[start_byte + 1];
    // get the 16 bits at start_byte offset
    tmp |= (bytes[start_byte] << 8);

    // remove the lower bits we dont want
    tmp >>= (sizeof(tmp) * 8) - new_at - len;

    // create the mask
    uint16_t mask = 0;
    for (int i = 0; i < len; ++i)
        mask |= 1 << i;

    // return the extracted bits
    return tmp & mask;
}
