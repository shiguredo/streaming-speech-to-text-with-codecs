#include "printer.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

std::shared_ptr<SharedPrinter> SharedPrinter::Create(const int output_num,
                                                     const bool _erase_output) {
  return std::make_shared<SharedPrinter>(output_num, _erase_output);
}

SharedPrinter::SharedPrinter(const int output_num, const bool _erase_output)
    : erase_output(_erase_output) {
  output_pairs.resize(output_num);
  for (auto &elem : output_pairs) {
    elem = std::make_pair("", "");
  }
}

void SharedPrinter::set_info(const int key, const std::string &info) {
  // get a lock
  std::lock_guard<std::mutex> _lock(mtx);

  // the first value is codec info
  output_pairs[key].first = info;
}

void SharedPrinter::set_output(const int key, const std::string &output) {
  // get a lock
  std::lock_guard<std::mutex> _lock(mtx);

  // the second output is printer main output
  output_pairs[key].second = output;
}

void SharedPrinter::print() {
  std::lock_guard<std::mutex> _lock(mtx);

  if (erase_output) {
    // erase output
    std::cout << "\033[2J";

    // back to top
    std::cout << "\033[0;0H";
  }

  // TODO: better output format
  {
    // main output

    // std::cout << "\n";
    for (const auto &elem : output_pairs) {
      const auto &info = elem.first;
      const auto &output = elem.second;

      std::cout << "[ " << info << " ]\n";

      {
        // erase previous output line
        std::cout << "\033[2K\033[G";
      }

      if (output != "") {
        std::cout << "output: " << output << "\n";
      } else {
        std::cout << "output: ...\n";
      }

      std::cout << "\n";
    }

    std::cout << std::endl;
  }
}