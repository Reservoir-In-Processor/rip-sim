#ifndef OPTIONS_H
#define OPTIONS_H
#include "CommonTypes.h"
#include <iostream>
#include <string>
enum BranchPredKind {
  No,
  One,
  Two,
  Gshare,
  // TODO: add interactive mode
  Interactive,
};
class Options {
private:
  std::string FileName;

  BranchPredKind BPKind;

  // cycle level interactive mode
  bool Interactive;

  // statistics dump
  bool Statistics;

  // statistics dump
  Address DRAMSize;

public:
  Options()
      : BPKind(No), Interactive(false), Statistics(false), DRAMSize(1 << 10) {}

  // return true if succeed.
  bool parse(int argc, char **argv) {
    if (argc < 2 || std::string(argv[3]) == "-h") {
      printHelp();
      return false;
    }

    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg[0] != '-') {
        FileName = arg;
      } else if (arg.substr(0, 2) == "-b") {
        std::string BPStr = arg.substr(3);
        if (BPStr != "no" && BPStr != "onebit" && BPStr != "twobit" &&
            BPStr != "gshare" && BPStr != "interactive") {
          std::cerr << "Invalid option for -b. Allowed options: no, onebit, "
                       "twobit, gshare and interactive.\n";
          return false;
        }
      } else if (arg == "-i") {
        Interactive = true;
      } else if (arg.substr(0, 10) == "--dram-size") {
        DRAMSize = std::stoll(arg.substr(11));
      } else if (arg == "--stats") {
        Statistics = true;
      } else {
        std::cerr << "Unknown option: " << arg << "\n";
        return false;
      }
    }

    return !FileName.empty();
  }

  void printHelp() {
    std::cerr
        << "Usage: rip-sim"
        << " <baremetal binary file name> "
           "-b=<onebit/twobit/gshare/interactive> [--dram-size=N] [--stats]\n"
        << "-b=<option> : Set branch prediction type (no, onebit, twobit, "
           "gshare and interactive)\n"
        << "--dram-size=N : Set DRAM size in kilobytes (N)\n"
        << "--stats : print statistics\n"
        << "-i : Additional flag\n";
  }

  Options(const Options &) = delete;
  Options &operator=(const Options &) = delete;

  inline const std::string &getFileName() { return FileName; }

  inline const BranchPredKind &getBPKind() { return BPKind; }

  inline const bool &getInteractive() { return Interactive; }

  inline const bool &getStatistics() { return Statistics; }

  inline const Address &getDRAMSize() { return DRAMSize; }
};
#endif