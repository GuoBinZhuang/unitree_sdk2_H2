#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace unitree::common {

struct TestOption {
  std::string name;
  int id;
};

inline int ConvertToInt(const std::string &str) {
  try {
    return std::stoi(str);
  } catch (const std::invalid_argument &) {
    return -1;
  } catch (const std::out_of_range &) {
    return -1;
  }
}

// Reads a test option name or id from stdin and stores the selection in
// `test_option`. Typing "list" prints every available option.
class UserInterface {
 public:
  UserInterface(const std::vector<TestOption> &option_list,
                TestOption *test_option)
      : option_list_(option_list), test_option_(test_option) {}

  void terminalHandle() {
    std::string input;
    std::getline(std::cin, input);

    if (input.compare("list") == 0) {
      for (const TestOption &option : option_list_) {
        std::cout << option.name << ", id: " << option.id << std::endl;
      }
    }

    for (const TestOption &option : option_list_) {
      if (input.compare(option.name) == 0 ||
          ConvertToInt(input) == option.id) {
        test_option_->id = option.id;
        test_option_->name = option.name;
        std::cout << "Test: " << test_option_->name
                  << ", test_id: " << test_option_->id << std::endl;
      }
    }
  }

 private:
  const std::vector<TestOption> &option_list_;
  TestOption *test_option_;
};

}  // namespace unitree::common
