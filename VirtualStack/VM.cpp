#include "VM.h"

uint8_t VM::execute(std::vector<uint8_t>& bytes)
{
    BytecodeParser bp;
    std::vector<Function> fn = bp.parse(bytes);
    // load the code into the program memory
    load_code(fn);

    bool halted = false;

    while (!halted) {
        execute_curr_inst();
    }
    
    // return the result
    return m_reg[8];
}

void VM::load_code(std::vector<Function>& fns)
{
    std::vector<uint16_t> fn_offsets;
    
    uint32_t *base = reinterpret_cast<uint32_t *>(m_code.get());
    uint32_t *curr = base;

    for (auto it = fns.begin(); it != fns.end(); ++it) {
        // add the offset
        fn_offsets.push_back(static_cast<uint16_t>((curr - base) / sizeof(uint32_t)));
        const std::vector<Instruction> &inst = (*it).getInstructions();
        
        for (auto iit = inst.cbegin(); iit != inst.cend(); ++iit) {
            uint32_t bytes = (*iit).encode();
            *curr = bytes;
            ++curr;
        }
    }

    uint32_t *pos = base;

    // perform fixups for function offsets
    while (pos != curr) {
        uint32_t inst = *pos;
        // matched a call instruction
        if ((inst & static_cast<uint32_t>(OpCode::CAL)) == static_cast<uint32_t>(OpCode::CAL)) {
            // extract the function id
            int fn = (inst >> 3) & 0xfff;
            // replace it with the code offset
            inst |= (static_cast<uint32_t>(fn_offsets[fn]) << 3);
            // add the parameter count
            inst |= (static_cast<uint8_t>(fns[fn].getParamCount()) << 15);
            // replace the instruction
            *pos = inst;
        }
        ++pos;
    }

}

void VM::next_inst()
{
    ++m_ip;
}

void VM::set_ip(uint16_t addr)
{
    m_ip = ret_addr_to_ip(addr);
}

void VM::build_stack_frame(uint32_t *new_ip, uint16_t * argptr, int num_args)
{
    // store the previous frame address
    *m_sp = bp_to_frame_addr(m_bp);
    ++m_sp;
    // new frame pointer is the current stack pointer
    m_bp = m_sp;
    // increment the stack pointer
    m_sp += 3;
    *(m_bp + 0) = bp_to_frame_addr(m_bp); // 0x0
    *(m_bp + 2) = ip_to_ret_addr(m_ip + 1); // 0x2

    // copy over any arguments
    for (int i = 0; i < num_args; ++i) {
        *m_sp = *argptr;
        ++m_sp;
        ++argptr;
    }

    *(m_bp + 1) = sp_to_stack_addr(m_sp); // 0x1
}

void VM::do_pop(int offset)
{
    // use a special register as the return value register
    m_reg[8] = static_cast<uint8_t>(*(m_bp + offset));
}

void VM::do_not(int src)
{
    m_reg[src] = ~m_reg[src];
}

void VM::do_add(int src, int dst)
{
    m_reg[src] += m_reg[dst];
}

void VM::do_and(int src, int dst)
{
    m_reg[src] &= m_reg[dst];
}

void VM::do_equ(int src)
{
    m_reg[src] = (m_reg[src] == 0) ? 1 : 0;
}

void VM::do_mov(AddrType src_type, uint8_t src_value, AddrType dst_type, uint8_t dst_value)
{
    uint8_t tmp = 0;

    switch (src_type) {
        case AddrType::Register:
            tmp = m_reg[src_value];
            break;
        case AddrType::Value:
            tmp = src_value;
            break;
        case AddrType::Stack:
            // handle program counter
            if (src_value == 0x2) {
                tmp = static_cast<uint8_t>(ip_to_ret_addr(m_ip) - *(m_bp + 2));
            }
            else {
                tmp = static_cast<uint8_t>(*(m_bp + static_cast<int>(src_value)));
            }
            break;
        case AddrType::Pointer:
            uint16_t tmpaddr = *(m_bp + static_cast<int>(src_value));
            tmp = static_cast<uint8_t>(*(stack_addr_to_sp(tmpaddr)));
            break;
    }

    switch (dst_type) {
        case AddrType::Register:
            m_reg[dst_value] = tmp;
            break;
        case AddrType::Stack:
            // handle program counter 
            if (dst_value == 0x2) {
                uint16_t new_ip = *(m_bp + 2) + static_cast<uint16_t>(tmp);
                set_ip(new_ip);
            }
            else {
                *(m_bp + static_cast<int>(dst_value)) = tmp;
            }
            break;
        case AddrType::Pointer:
            uint16_t tmpaddr = *(m_bp + static_cast<int>(dst_value));
            *(stack_addr_to_sp(tmpaddr)) = tmp;
            // increment the stack pointer
            *(m_bp + 1) = (*(m_bp + 1)) + 1;
            break;
    }
}

bool VM::do_ret()
{
    // last instruction in main
    if (m_bp - reinterpret_cast<uint16_t *>(m_data.get()) == 0) {
        return true;
    }

    uint16_t ret_addr = *(m_bp + 2);
    // set the frame pointer to the old one
    uint16_t *tmp_bp = frame_addr_to_bp(*(m_bp - 1));
    // set the instruction pointer
    set_ip(ret_addr);
    // get the return value from the return register
    *(m_bp - 1) = m_reg[8];
    // last instruction in a non-main function
    m_bp = tmp_bp;
    return false;
}

void VM::do_cal(uint32_t * new_ip, uint16_t * sp_args, uint8_t num_args)
{
    build_stack_frame(new_ip, sp_args, num_args);
    m_ip = new_ip;
}

uint32_t * VM::ret_addr_to_ip(uint16_t addr)
{
    return reinterpret_cast<uint32_t *>(m_code.get()) + (static_cast<uint32_t>(addr) * sizeof(uint32_t));
}

uint16_t VM::ip_to_ret_addr(uint32_t * ip)
{
    return static_cast<uint16_t>((ip - reinterpret_cast<uint32_t *>(m_code.get())) / sizeof(uint32_t));
}

uint16_t VM::bp_to_frame_addr(uint16_t * bp)
{
    return static_cast<uint16_t>((bp - reinterpret_cast<uint16_t *>(m_data.get())) / sizeof(uint16_t));
}

uint16_t * VM::frame_addr_to_bp(uint16_t addr)
{
    return reinterpret_cast<uint16_t *>(m_data.get()) + (static_cast<uint16_t>(addr) * sizeof(uint16_t));
}

uint16_t VM::sp_to_stack_addr(uint16_t * sp)
{
    return static_cast<uint16_t>((sp - reinterpret_cast<uint16_t *>(m_data.get())) / sizeof(uint16_t));
}

uint16_t * VM::stack_addr_to_sp(uint16_t addr)
{
    return reinterpret_cast<uint16_t *>(m_data.get()) + (static_cast<uint16_t>(addr) * sizeof(uint16_t));
}

bool VM::execute_curr_inst()
{
    uint32_t bytes = *m_ip;

    uint8_t op = bytes & 0x07;
    bytes >>= 3;
    uint8_t dst_value, src_value;
    AddrType dst_type, src_type;

    switch (static_cast<OpCode>(op)) {
        case OpCode::RET:
            if (do_ret()) {
                return true;
            }
            break;
        case OpCode::NOT:
            bytes >>= 2;
            dst_value = bytes & 0x7;
            do_not(dst_value);
            break;
        case OpCode::EQU:
            bytes >>= 2;
            dst_value = bytes & 0x7;
            do_equ(dst_value);
            break;
        case OpCode::POP:
            bytes >>= 2;
            dst_value = bytes & 0x7f;
            do_pop(dst_value);
            break;
        case OpCode::ADD:
            bytes >>= 2;
            dst_value = bytes & 0x7;
            bytes >>= 5;
            src_value = bytes & 0x7;
            do_add(src_value, dst_value);
        case OpCode::AND:
            bytes >>= 2;
            dst_value = bytes & 0x7;
            bytes >>= 5;
            src_value = bytes & 0x7;
            do_and(src_value, dst_value);
        case OpCode::MOV:
            dst_type = static_cast<AddrType>(bytes & 0x3);
            bytes >>= 2;
            dst_value = 0;

            switch (dst_type) {
                case AddrType::Value:
                    dst_value = bytes & 0xff;
                    bytes >>= 8;
                    break;
                case AddrType::Register:
                    dst_value = bytes & 0x7;
                    bytes >>= 3;
                    break;
                case AddrType::Stack:
                    dst_value = bytes & 0x7f;
                    bytes >>= 7;
                    break;
                case AddrType::Pointer:
                    dst_value = bytes & 0x7f;
                    bytes >>= 7;
                    break;
            }

            src_type = static_cast<AddrType>(bytes & 0x3);
            bytes >>= 2;
            src_value = 0;

            switch (src_type) {
                case AddrType::Value:
                    src_value = bytes & 0xff;
                    bytes >>= 8;
                    break;
                case AddrType::Register:
                    src_value = bytes & 0x7;
                    bytes >>= 3;
                    break;
                case AddrType::Stack:
                    src_value = bytes & 0x7f;
                    bytes >>= 7;
                    break;
                case AddrType::Pointer:
                    src_value = bytes & 0x7f;
                    bytes >>= 7;
                    break;
            }

            do_mov(src_type, src_value, dst_type, dst_value);
            break;
        case OpCode::CAL:
            uint16_t fn_offset = static_cast<uint16_t>(bytes & 0xfff);
            bytes >>= 12;
            src_value = static_cast<uint8_t>(bytes & 0xf);
            bytes >>= 6;
            dst_value = bytes & 0x7f;
            uint16_t *params_ptr = m_bp + static_cast<int>(dst_value);
            uint32_t *new_ip = reinterpret_cast<uint32_t *>(m_code.get()) + static_cast<int>(fn_offset);
            do_cal(new_ip, params_ptr, src_value);
            break;
    }

    // increment the instruction pointer
    ++m_ip;

    return false;
}



