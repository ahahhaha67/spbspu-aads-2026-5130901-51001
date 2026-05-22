#include <exception>
#include <fstream>
#include <iostream>
#include "commands.hpp"

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cerr << "ERROR: filename argument required\n";
    return 1;
  }
  std::ifstream file(argv[1]);
  if (!file.is_open())
  {
    std::cerr << "ERROR: cannot open file\n";
    return 1;
  }
  krivoshapov::DatasetStore store(16);
  try
  {
    krivoshapov::loadDatasets(file, store);
    krivoshapov::runCommands(std::cin, std::cout, store);
  }
  catch (const std::exception &e)
  {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
