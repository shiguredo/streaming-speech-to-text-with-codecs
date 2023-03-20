#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

class PrinterInterface {
 public:
  virtual ~PrinterInterface() = default;
  virtual void print() = 0;
  virtual void set_output(const int key, const std::string& output) = 0;
};

class SharedPrinter : public PrinterInterface {
 public:
  static std::shared_ptr<SharedPrinter> Create(const int output_num, const bool _erase_output);
  SharedPrinter(const int output_num, const bool _erase_output);
  ~SharedPrinter() = default;

  void print();
  void set_output(const int key, const std::string& output);

  void set_info(const int key, const std::string& info);

 private:
  std::mutex mtx;
  bool erase_output = true;

  // Each of pairs has codec info in first value and output in second value
  std::vector<std::pair<std::string, std::string>> output_pairs;
};
