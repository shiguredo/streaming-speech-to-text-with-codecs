#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "codec/applier_interface.h"

namespace codec {
namespace util {

template <typename T>
T crop(const T lower, const T upper, const T val);

std::vector<int16_t> pcmf32_to_wavi16(const std::vector<float> &pcmf32);

std::vector<float> wavi16_to_pcmf32(const std::vector<int16_t> &wav16);

std::unique_ptr<CodecApplierInterface> get_codec_applier(
    const std::string &codec_name, const int bitrate, const int sample_rate,
    const std::string &lyra_model_path);

}  // namespace util
}  // namespace codec