#pragma once

#include <memory>
#include <vector>

#include "applier_interface.h"

namespace codec {

class NoCodecApplier : public CodecApplierInterface {
 public:
  static std::unique_ptr<NoCodecApplier> Create();

  std::vector<float> apply(std::vector<float> &&original);

  NoCodecApplier();
  ~NoCodecApplier() = default;
};

}  // namespace codec