#pragma once

#include <memory>
#include <vector>

namespace codec {

class CodecApplierInterface {
 public:
  virtual ~CodecApplierInterface() = default;

  virtual std::vector<float> apply(std::vector<float> &&original) = 0;
};

}  // namespace codec