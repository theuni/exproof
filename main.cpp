#include <cstdio>
#include <vector>

#include "crypto/sha256.h"
#include "exproof.hpp"

int main(int, const char**)
{
    SHA256AutoDetect();

    auto ret = Program({
        {opcode::add, reg::r0, 3},
        {opcode::add, reg::r1, 7},
        {opcode::add, reg::r0, reg::r1},
        {opcode::add, reg::r1, reg::r0},
        {opcode::sub, reg::r1, 5},
        {opcode::sub, reg::r1, reg::r0},
        {opcode::hlt},
    }).Run();

    printf("registers: \n%s\n", ret.ToString().c_str());
    return 0;
}
