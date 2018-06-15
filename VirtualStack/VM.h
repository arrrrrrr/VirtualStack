#pragma once
#include "Common.h"


template<class T>
class Stack {
public:
    Stack(int size)
        : m_data(new uint8_t[size]), m_sp(m_data), m_size(size) {
    }

    ~Stack() {
        delete m_stack;
    }

    void push(T value) {
        if (m_sp > m_stack + (sizeof(T) * m_size)) {
            throw std::runtime_error("Stack Overflow!");
        }

        *m_sp = value;
        ++m_sp;
    }

    T pop() {
        if ((m_sp - m_stack - 1) < 0) {
            throw std::runtime_error("Stack underflow!");
        }

        T value = *(m_sp - 1);
        --m_sp;
    }

    T top() {
        if ((m_sp - m_stack - 1) < 0) {
            throw std::runtime_error("Stack underflow!");
        }

        return *(m_sp - 1);
    }

    T ptr() {
        return m_sp;
    }

    T& operator[](std::size_t idx) {
        if (m_sp)
    }


    int size() {
        return m_sp - m_stack - 1;
    }

private:
    T * m_stack;
    T *m_sp;
    int m_size;
};

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
    int m_size;
};

class FunctionTable {
public:
    FunctionTable() {}
    ~FunctionTable() {}

    void addFunction(Function f) {
        m_functions.push_back(f);
    }

    Function& operator[](std::size_t idx) {
        return m_functions[idx];
    }

    const Function& operator[](std::size_t idx) const {
        return m_functions[idx];
    }


private:
    std::vector<Function> m_functions;

};

class VM
{
public:
    VM & instance() {
        static VM instance;
        return instance;
    }
    
    RegisterFile<uint8_t>& registers() {
        return m_reg;
    }
      
    Stack<uint8_t>& stack() {
        return m_stack;
    }
        
    uint8_t get_program_ctr() {
        return m_pc;
    }

    void inc_program_ctr() {
        m_pc++;
    }
    
    uint8_t get_result() {

    }

    ~VM() {}

protected:
    VM() : m_ft(), m_stack(128), m_reg(8), m_pc(0), m_execres(0) {}
    
    FunctionTable m_ft;
    Stack<uint8_t> m_stack;
    RegisterFile<uint8_t> m_reg;
    uint8_t m_pc;
    uint8_t m_execres;
};

