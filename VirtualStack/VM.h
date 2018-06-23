#pragma once
#include "Common.h"
#include "BytecodeParser.h"

template <class T>
class RegisterFile {
public:
    RegisterFile(int size)
        : m_reg(new T[size]), m_size(size)
    {}

    ~RegisterFile() {
        delete m_reg;
    }

    T& operator[](std::size_t idx) {
        if (idx >= m_size - 1) {
            throw std::runtime_error("register out of bounds");
        }
        return m_reg[idx];
    }

    const T& operator[](std::size_t idx) const {
        if (idx >= m_size - 1) {
            throw std::runtime_error("register out of bounds");
        }
        return m_reg[idx];
    }

private:
    T * m_reg;
    size_t m_size;
};

class VM
{
public:
    static VM & instance() {
        static VM inst;
        return inst;
    }
    
    RegisterFile<uint8_t>& registers() {
        return m_reg;
    }
    
    uint16_t *sp() {
        return m_sp;
    }

    uint16_t *bp() {
        return m_bp;
    }
        
    uint32_t *ip() {
        return m_ip;
    }

    uint8_t execute(std::vector<uint8_t>& bytes);
    
    ~VM() {}

protected:
    VM()
        : m_code(new char[8192]{ 0 }), 
          m_data(new char[4096]{ 0 }),
          m_reg(9),
          m_ip(reinterpret_cast<uint32_t *>(m_code.get())),
          m_bp(reinterpret_cast<uint16_t *>(m_data.get())),
          m_sp(reinterpret_cast<uint16_t *>(m_data.get()))
    {}

    void load_code(std::vector<Function>& fns);
    void next_inst();
    void set_ip(uint16_t addr);

    void build_stack_frame(uint32_t *new_ip, uint16_t *argptr, int num_args);
    
    void do_pop(int offset);
    void do_not(int src);
    void do_add(int src, int dst);
    void do_and(int src, int dst);
    void do_equ(int src);
    void do_mov(AddrType src_type, uint8_t src_value, AddrType dst_type, uint8_t dst_value);
    bool do_ret();
    void do_cal(uint32_t *new_ip, uint16_t *sp_args, uint8_t num_args);

    uint32_t *ret_addr_to_ip(uint16_t addr);
    uint16_t ip_to_ret_addr(uint32_t *ip);
    uint16_t bp_to_frame_addr(uint16_t *bp);
    uint16_t *frame_addr_to_bp(uint16_t addr);
    uint16_t sp_to_stack_addr(uint16_t *sp);
    uint16_t *stack_addr_to_sp(uint16_t addr);
    
    bool execute_curr_inst();

    std::unique_ptr<char> m_code;
    std::unique_ptr<char> m_data;
    RegisterFile<uint8_t> m_reg;
    uint32_t *m_ip;
    uint16_t *m_bp;
    uint16_t *m_sp;
};

