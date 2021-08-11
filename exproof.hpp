// Copyright (c) 2021 Cory Fields
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef EXPROOF_H_
#define EXPROOF_H_

#include <cstdint>
#include <variant>
#include <vector>
#include <array>
#include <string>

enum class opcode
{
    add = 0,
    sub,
    hlt
};

enum class reg
{
    r0 = 0,
    r1
};

using operand = std::variant<std::monostate, reg, uint64_t>;

class Instruction
{
public:
    constexpr Instruction(opcode op) : m_opcode{op}{}
    constexpr Instruction(opcode op, operand a) : m_opcode{op}, m_operand1{a}{}
    constexpr Instruction(opcode op, operand a, operand b) : m_opcode{op}, m_operand1{a}, m_operand2{b}{};
    const opcode &GetOpcode() const;
    const operand& GetOperand1() const;
    const operand& GetOperand2() const;
private:
    opcode m_opcode;
    operand m_operand1;
    operand m_operand2;
};


struct Registers
{
    std::string ToString() const;

    std::array<uint64_t, 2> m_gen{0};
    uint64_t m_pc{0};
    std::array<std::byte, 32> m_hash{};
};

class Program
{
public:
    Program(std::vector<Instruction> instructions) : m_instructions(std::move(instructions)){}
    Registers Run();
private:
    Instruction Fetch() const;
    void UpdateHash();
    void Add(const operand& a, const operand& b);
    void Sub(const operand& a, const operand& b);

    Registers m_regs;
    std::vector<Instruction> m_instructions;
};

#endif // EXPROOF_H_
