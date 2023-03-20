#include "codec/util.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "codec/applier_interface.h"
#include "codec/lyra_applier.h"
#include "codec/no_codec_applier.h"
#include "codec/opus_applier.h"

namespace codec {
namespace util {

template <typename T>
T crop(const T lower, const T upper, const T val) {
  return std::min(upper, std::max(lower, val));
}

std::vector<int16_t> pcmf32_to_wavi16(const std::vector<float> &pcmf32) {
  const int length = pcmf32.size();
  std::vector<int16_t> output(length);

  for (int i = 0; i < length; i++) {
    const auto old_val = crop(-1.0f, 1.0f, pcmf32[i]);
    const int16_t new_val =
        crop(-32768, 32767, static_cast<int>(old_val * (1 << 15)));
    output[i] = new_val;
  }

  return output;
}

std::vector<float> wavi16_to_pcmf32(const std::vector<int16_t> &wav16) {
  const int length = wav16.size();
  std::vector<float> output(length);

  for (int i = 0; i < length; i++) {
    const auto old_val = wav16[i];
    const float new_val = crop(-1.0f, 1.0f, float(old_val) / (32768));
    output[i] = new_val;
  }

  return output;
}

std::unique_ptr<CodecApplierInterface> get_codec_applier(
    const std::string &codec_name, const int bitrate, const int sample_rate,
    const std::string &lyra_model_path) {
  if (codec_name == "opus") {
    return OpusApplier::Create(bitrate, sample_rate);
  } else if (codec_name == "lyra") {
    return LyraApplier::Create(sample_rate, 1, bitrate, false, lyra_model_path);
  } else {
    return NoCodecApplier::Create();
  }
}

}  // namespace util
}  // namespace codec