#include "routine.h"

#include "Logger.h"
#include "keyboard.h"

#include <fstream>
#include <sstream>
#include <vector>

Instruction::Instruction(int key, int seconds, std::string description)
: _key{key}, _wait{seconds, 0}, _description{description}
{};

Routine::Routine(const std::string instructionsFileName)
{
  std::ifstream fileStream;
  fileStream.open(instructionsFileName.c_str(), std::ifstream::in);
  if (!fileStream.is_open())
  {
    LOGGER->LOG(1, LOGLEVEL_ERROR, "The file %s cannot be opened, no instructions to parse.", instructionsFileName.c_str());
  }

  std::string line;
  while (std::getline(fileStream, line, '\n'))
  {
    if (line[0] != '#' && line[0] != ' ' && line[0] != '\n') {   // Skip comments or empty lines
      
      std::istringstream lineStream(line);
      std::string key{}, time{}, description{};
      if (std::getline(lineStream, key, ' ')) {
        std::getline(lineStream, time, ' ');
        std::getline(lineStream, description);
        try
        {
          int keyValue = Keys.at(key);
          int seconds = atoi(time.c_str());
          instructions.emplace_back(keyValue, seconds, description);
        }
        catch(const std::exception& e)
        {
          LOGGER->LOG(1, LOGLEVEL_ERROR, "%s, Invalid instruction %s, skipped", e.what(), key.c_str());
        }
      }
    }
  }

  _nextInstruction = instructions.begin();

  LOGGER->LOG(1, LOGLEVEL_INFO, "End of instructions parse, there are %d instructions.", instructions.size());
}

Instruction Routine::getNextInstruction()
{
  Instruction nextInstruction = *_nextInstruction.base();
  _nextInstruction++;

  if(_nextInstruction == instructions.end()) {
    _iteration++;
    LOGGER->LOG(1, LOGLEVEL_INFO, "End of routine. Iteration %ld will begin.", _iteration);
    _nextInstruction = instructions.begin();
  }

  return nextInstruction;
}
