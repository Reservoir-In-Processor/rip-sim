#include <Simulator/Simulator.h>
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
  Simulator Sim(Files, /*DRAMSize = */ 1LL << 28,
                /* DRAMBase = */ 0x0000,
                /* SPIvalue = */ 1LL << 25);
  Sim.run();
  Sim.dumpGPRegs();
  Sim.getCSRs().dump();

  return 0;
}
