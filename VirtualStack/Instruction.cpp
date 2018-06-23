#include "Instruction.h"

uint32_t Instruction::encode() const
{
    uint32_t bytes = 0;
    int bits = 0;

    bytes |= static_cast<uint8_t>(opcode);
    bits += 3;

    switch (opcode) {
        case OpCode::RET:
            break;
        case OpCode::NOT:
        case OpCode::EQU:
            bytes |= (static_cast<uint8_t>(dst_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(dst) << bits);
            break;
        case OpCode::POP:
            bytes |= (static_cast<uint8_t>(dst_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(dst) << bits);
            break;
        case OpCode::ADD:
        case OpCode::AND:
            bytes |= (static_cast<uint8_t>(dst_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(dst) << bits);
            bits += 3;
            bytes |= (static_cast<uint8_t>(src_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(src) << bits);
            break;
        case OpCode::MOV:
            bytes |= (static_cast<uint8_t>(dst_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(dst) << bits);
            
            switch (dst_type) {
                case AddrType::Value:
                    bits += 8; break;
                case AddrType::Register:
                    bits += 3; break;
                case AddrType::Pointer:
                case AddrType::Stack:
                    bits += 7; break;
            }

            bytes |= (static_cast<uint8_t>(src_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(src) << bits);
            break;
        case OpCode::CAL:
            // 001 | xxxxxxxxxxxx | xxxx   | xx        | xxxxxxxx
            // op  | code_offset  | params | addr_type | addr

            // 12 bit offset to be patched in later
            bytes |= (static_cast<uint8_t>(dst) << bits);
            bits += 12;
            bytes |= (static_cast<uint8_t>(0) << bits);
            bits += 4;
            bytes |= (static_cast<uint8_t>(src_type) << bits);
            bits += 2;
            bytes |= (static_cast<uint8_t>(src) << bits);
            break;
    }

    return bytes;
}
