#include "exproof.hpp"

#include <cassert>

const opcode& Instruction::GetOpcode() const
{
    return m_opcode;
}
const operand& Instruction::GetOperand1() const
{
    return m_operand1;
}
const operand& Instruction::GetOperand2() const
{
    return m_operand2;
}

std::string Registers::ToString() const {
    std::string ret;
    ret.reserve((1 + m_gen.size()) * 16);
    for (size_t i = 0; i < m_gen.size(); i++) {
        ret += "r" + std::to_string(i) + ": " + std::to_string(m_gen[i]) + "\n";
    }
    ret += "pc: " + std::to_string(m_pc) + "\n";
    return ret;
}

Registers Program::Run()
{
    bool halted = false;
    while (!halted && m_regs.m_pc < m_instructions.size()) {
        auto instruction(Fetch());
        auto op = instruction.GetOpcode();
        switch(op) {
            case opcode::hlt:
                halted = true;
                break;
            case opcode::add:
                Add(instruction.GetOperand1(), instruction.GetOperand2());
                break;
            case opcode::sub:
                Sub(instruction.GetOperand1(), instruction.GetOperand2());
                break;
        }
        m_regs.m_pc++;
    }
    return m_regs;
}

Instruction Program::Fetch() const
{
    assert(m_regs.m_pc < m_instructions.size());
    return m_instructions[m_regs.m_pc];
}

void Program::Add(const operand& a, const operand& b)
{
    assert(std::holds_alternative<reg>(a));
    assert(!std::holds_alternative<std::monostate>(b));
    uint64_t* lhs = m_regs.m_gen.data() + static_cast<uint64_t>(std::get<reg>(a));

    if (std::holds_alternative<reg>(b)) {
        *lhs += m_regs.m_gen[static_cast<uint64_t>(std::get<reg>(b))];
    } else {
        *lhs += std::get<uint64_t>(b);
    }
}

void Program::Sub(const operand& a, const operand& b)
{
    assert(std::holds_alternative<reg>(a));
    assert(!std::holds_alternative<std::monostate>(b));
    uint64_t* lhs = m_regs.m_gen.data() + static_cast<uint64_t>(std::get<reg>(a));

    if (std::holds_alternative<reg>(b)) {
        *lhs -= m_regs.m_gen[static_cast<uint64_t>(std::get<reg>(b))];
    } else {
        *lhs -= std::get<uint64_t>(b);
    }
}
