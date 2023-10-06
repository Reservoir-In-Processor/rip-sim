#include "CSR.h"
#include <cassert>

void CSRs::write(CSRAddress Ad, CSRVal Val) {
  if (CSR_SIZE <= Ad)
    assert(false && "CSR: invalid address.");
  States[Ad] = Val;
}

CSRVal CSRs::read(CSRAddress Ad) {
  if (CSR_SIZE <= Ad)
    assert(false && "CSR: invalid address.");
  return States[Ad];
}