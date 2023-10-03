#ifdef DEBUG

#ifndef DEBUG_H
#define DEBUG_H
#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>

extern void dumpInstVal(unsigned Val);

extern void debugInstOnAsm(const std::vector<std::string> Toks, unsigned Val);

#endif
#endif
