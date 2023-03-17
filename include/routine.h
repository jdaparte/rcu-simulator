#pragma once

#include <string>
#include <vector>


/**
 * @brief Vector with instructions set to be repeted indefinitely.
 *    Instruction Structure:  
 *      - First field: Key to be pressed and released
 *      - Second field: Waiting time after presing the key (seconds)
 *      - Third field: Description to be printed in logs (optional)
 */

struct Instruction
{
  Instruction(int key, int seconds, std::string description);

  int _key {0};
  timespec _wait {0, 0};
  std::string _description {""};
};

struct Routine
{
  Routine(const std::string instructionsFileName);
  const Instruction& getNextInstruction();
  inline uint8_t getInstructionsSize() { return instructions.size(); }

private:
  std::vector<Instruction> instructions {};
  std::vector<Instruction>::iterator _nextInstruction;
  std::uint64_t _iteration {0};
};