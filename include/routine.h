struct Instruction
{
  Instruction(int key, int wait, std::string description);

  int _key {0};
  int _wait {0};
  std::string _description {""};
};