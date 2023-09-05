#ifndef ASMPARSER_H
#define ASMPARSER_H
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

class AsmParser {
private:
  using Address = unsigned;
  std::istream &Is;
  std::vector<std::string> Toks;
  int LineCounter;
  Address PC;
  std::map<std::string, Address> Labels;

  // define on AsmParser.cpp
  std::vector<std::string> parseLineInner(const std::string &str);

public:
  AsmParser(const AsmParser &) = delete;
  AsmParser &operator=(const AsmParser &) = delete;

  AsmParser(std::istream &is) : Is(is), LineCounter(0), PC(0) {
    // initialize Labels
    std::string line;
    while (std::getline(Is, line)) {
      if (line.find(':') != std::string::npos) {
        std::string lab = line.substr(0, line.find(':'));
        std::cerr << "parsed label = " << lab << "\n";
        Labels.insert({lab, PC});
        continue;
      }
      PC += 4;
    }
    Is.clear();
    Is.seekg(0, std::ios::beg);
    PC = 0;
  }

  /// read line
  bool parseLine();
  const std::vector<std::string> &getTokens() const { return Toks; }
  const std::map<std::string, Address> &getLabels() const { return Labels; }

  int getLineCounter() const { return LineCounter; }
};
#endif