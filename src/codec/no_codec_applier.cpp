#include "codec/no_codec_applier.h"

#include <memory>
#include <vector>

namespace codec {

std::unique_ptr<NoCodecApplier> NoCodecApplier::Create() {
  return std::make_unique<NoCodecApplier>();
}

std::vector<float> NoCodecApplier::apply(std::vector<float> &&original) {
  return original;
}

NoCodecApplier::NoCodecApplier() {}

}  // namespace codec
