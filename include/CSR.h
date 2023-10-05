#ifndef CSR_H
#define CSR_H

#include <cstdint>
const unsigned CSR_SIZE = 4096;
using CSRVal = std::int32_t;

class CSRs {
private:
  CSRVal States[CSR_SIZE];

public:
  CSRs(const CSRs &) = delete;
  CSRs &operator=(const CSRs &) = delete;

  CSRs() : States{0} {}
};

#endif