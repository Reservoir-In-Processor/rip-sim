#ifndef OPTIONS_H
#define OPTIONS_H
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

public:
  Options() : BPKind(No), Interactive(false) {}

  // return true if succeed.
  bool parse(int argc, char **argv) {
    if (argc < 2) {
      std::cerr << "Usage: " << argv[0]
                << " <baremetal binary file name> "
                   "-b=<no/onebit/twobit/gshare/interactive> [-i]\n";
      return false;
    }

    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-i") {
        Interactive = true;
      } else if (arg.substr(0, 2) == "-b") {
        std::string BPStr = arg.substr(3);
        if (BPStr != "no" && BPStr != "onebit" && BPStr != "twobit" &&
            BPStr != "gshare" && BPStr != "interactive") {
          std::cerr << "Invalid option for -b. Allowed options: no, onebit, "
                       "twobit, gshare and interactive.\n";
          return false;
        }
      } else if (arg[0] != '-') {
        FileName = arg;
      } else {
        std::cerr << "Unknown option: " << arg << "\n";
        return false;
      }
    }

    return !FileName.empty();
  }

  Options(const Options &) = delete;
  Options &operator=(const Options &) = delete;

  inline const bool &getInteractive() { return Interactive; }
  inline const BranchPredKind &getBPKind() { return BPKind; }
  inline const std::string &getFileName() { return FileName; }
};
#endif