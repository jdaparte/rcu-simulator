#include "routine.h"

Instruction::Instruction(int key, int wait, std::string description)
: _key{key}, _wait{wait}, _description{description}
{};