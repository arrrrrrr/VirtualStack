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

#define DEBUG 0

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
