#include "exproof.hpp"
#include "crypto/sha256.h"

#include <cassert>

namespace {

std::string ToHex(const unsigned char *data, size_t len)
{
    static constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string ret;
    ret.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        ret.push_back(hexmap[(data[i] & 0xF0) >> 4]);
        ret.push_back(hexmap[data[i] & 0x0F]);
    }
    return ret;
}

} // anon namespace

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
    ret.reserve(256);
    for (size_t i = 0; i < m_gen.size(); i++) {
        ret += "r" + std::to_string(i) + ": " + std::to_string(m_gen[i]) + "\n";
    }
    ret += "pc: " + std::to_string(m_pc) + "\n";
    ret += "hash: " + ToHex(reinterpret_cast<const unsigned char*>(m_hash.data()), m_hash.size());
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
        UpdateHash();
        m_regs.m_pc++;
    }
    return m_regs;
}

void Program::UpdateHash()
{
    CSHA256 sha;
    sha.Write(reinterpret_cast<const unsigned char*>(m_regs.m_hash.data()), m_regs.m_hash.size());
    for (const auto& reg : m_regs.m_gen) {
        sha.Write(reinterpret_cast<const unsigned char*>(&reg), sizeof(reg));
    }
    sha.Write(reinterpret_cast<const unsigned char*>(&m_regs.m_pc), sizeof(m_regs.m_pc));
    sha.Finalize(reinterpret_cast<unsigned char*>(m_regs.m_hash.data()));
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
