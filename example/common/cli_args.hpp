#pragma once

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace unitree::common {

// Parses whitespace separated floats, e.g. "0.3 0 0.5".
inline std::vector<float> StringToFloatVector(const std::string &str) {
  std::vector<float> result;
  std::stringstream ss(str);
  float num;
  while (ss >> num) {
    result.push_back(num);
    // ignore any trailing whitespace
    ss.ignore();
  }
  return result;
}

// Parses "--key=value" / "--key" command line arguments into `args`, keeping
// the defaults already present in it for keys that are not passed.
inline void ParseArgs(int argc, char const *argv[],
                      std::map<std::string, std::string> &args) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.substr(0, 2) != "--") {
      continue;
    }
    size_t pos = arg.find("=");
    std::string key, value;
    if (pos != std::string::npos) {
      key = arg.substr(2, pos - 2);
      value = arg.substr(pos + 1);

      if (value.front() == '"' && value.back() == '"') {
        value = value.substr(1, value.length() - 2);
      }
    } else {
      key = arg.substr(2);
      value = "";
    }
    args[key] = value;
  }
}

}  // namespace unitree::common
