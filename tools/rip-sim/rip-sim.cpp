#include <RIPSimulator/RIPSimulator.h>
#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <baremetal binary file name>"
              << "\n";
    return 1;
  }
  const std::string FileName = argv[1];
  std::string BaseNoExt = FileName.substr(0, FileName.find_last_of('.'));
  auto Files = std::ifstream(FileName);
  RIPSimulator RipSim(Files);
  RipSim.run();

  return 0;
}
