// TODO: implement method in this
#include <AsmParser.h>

std::vector<std::string> AsmParser::parseLineInner(const std::string &str) {
  std::vector<std::string> result;
  std::string temp;
  std::stringstream local_ss(str);
  // TODO: handle comment
  while (std::getline(local_ss, temp, ',')) {
    std::stringstream item_ss(temp);
    std::string item;
    while (item_ss >> item) {
      result.push_back(item);
    }
  }

  return result;
}

bool AsmParser::parseLine() {
  std::string line;
  while (std::getline(Is, line)) {
    ++LineCounter;
    if (line.find(':') != std::string::npos)
      continue;
    Toks = parseLineInner(line);
    return true;
  }
  return false;
}