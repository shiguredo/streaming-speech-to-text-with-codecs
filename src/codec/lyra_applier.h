#pragma once

#include <memory>
#include <vector>

#include "applier_interface.h"
#include "lyra/lyra_decoder.h"
#include "lyra/lyra_encoder.h"

namespace codec {

class LyraApplier : public CodecApplierInterface {
 public:
  static std::unique_ptr<LyraApplier> Create(
      int sample_rate_hz, int num_channels, int bitrate, bool enable_dtx,
      const ghc::filesystem::path &model_path);

  std::vector<float> apply(std::vector<float> &&original);

  ~LyraApplier() = default;
  LyraApplier(std::unique_ptr<chromemedia::codec::LyraEncoder> &&e,
              std::unique_ptr<chromemedia::codec::LyraDecoder> &&d,
              int sample_rate_hz, int num_channels, int bitrate,
              bool enable_dtx);

  const int sample_rate_hz;
  const int num_channels;
  const int bitrate;
  const bool enable_dtx;

 private:
  const std::unique_ptr<chromemedia::codec::LyraEncoder> encoder;
  const std::unique_ptr<chromemedia::codec::LyraDecoder> decoder;
};

}  // namespace codec