#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "applier_interface.h"
#include "opus/opus.h"

namespace codec {

class OpusApplier : public CodecApplierInterface {
 public:
  ~OpusApplier();
  OpusApplier(const int32_t bitrate, const int sample_rate, OpusEncoder *e,
              OpusDecoder *d);

  static std::unique_ptr<OpusApplier> Create(const int32_t bitrate,
                                             const int sample_rate);

  std::vector<float> apply(std::vector<float> &&original);

 private:
  const int32_t bitrate;
  const int sample_rate;
  OpusEncoder *encoder;
  OpusDecoder *decoder;
};

}  // namespace codec
