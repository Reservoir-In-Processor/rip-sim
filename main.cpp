#include <BinaryEmitter.h>
#include <cassert>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>"
              << "\n";
    return 1;
  }
  const std::string FileName = argv[1];
  std::string BaseNoExt = FileName.substr(0, FileName.find_last_of('.'));
  auto Files = std::ifstream(FileName);
  BinaryEmitter BE(Files);

  std::ofstream Out(BaseNoExt + ".bin", std::ios::out | std::ios::binary);
  BE.emitBinary(Out);
  return 0;
}
