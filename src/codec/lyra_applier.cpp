#include "codec/lyra_applier.h"

#include <memory>
#include <vector>

#include "absl/types/span.h"
#include "codec/util.h"
#include "lyra/lyra_config.h"
#include "lyra/lyra_decoder.h"
#include "lyra/lyra_encoder.h"

namespace codec {

std::unique_ptr<LyraApplier> LyraApplier::Create(
    int sample_rate_hz, int num_channels, int bitrate, bool enable_dtx,
    const ghc::filesystem::path &model_path) {
  auto e = chromemedia::codec::LyraEncoder::Create(
      sample_rate_hz, num_channels, bitrate, enable_dtx, model_path);
  auto d = chromemedia::codec::LyraDecoder::Create(sample_rate_hz, num_channels,
                                                   model_path);

  if (e == nullptr || d == nullptr) return nullptr;

  return std::make_unique<LyraApplier>(std::move(e), std::move(d),
                                       sample_rate_hz, num_channels, bitrate,
                                       enable_dtx);
}

std::vector<float> LyraApplier::apply(std::vector<float> &&original) {
  const auto original_wav = util::pcmf32_to_wavi16(original);

  const int num_samples_per_packet = sample_rate_hz / encoder->frame_rate();

  // encode
  std::vector<uint8_t> encoded_features;
  for (int wav_iterator = 0;
       wav_iterator + num_samples_per_packet <= original_wav.size();
       wav_iterator += num_samples_per_packet) {
    auto encoded = encoder->Encode(absl::MakeConstSpan(
        &original_wav.at(wav_iterator), num_samples_per_packet));
    encoded_features.insert(encoded_features.begin(), encoded.value().begin(),
                            encoded.value().end());
  }

  // decode
  std::vector<int16_t> decoded_audio;
  const auto packet_size =
      chromemedia::codec::BitrateToPacketSize(encoder->bitrate());
  for (int encoded_idx = 0; encoded_idx < encoded_features.size();
       encoded_idx += packet_size) {
    const auto encoded_packet =
        absl::MakeConstSpan(encoded_features.data() + encoded_idx, packet_size);
    decoder->SetEncodedPacket(encoded_packet);

    int samples_decoded_so_far = 0;
    while (samples_decoded_so_far < num_samples_per_packet) {
      auto decoded = decoder->DecodeSamples(num_samples_per_packet);
      samples_decoded_so_far += decoded->size();
      decoded_audio.insert(decoded_audio.end(), decoded.value().begin(),
                           decoded.value().end());
    }
  }

  return util::wavi16_to_pcmf32(original_wav);
}

LyraApplier::LyraApplier(std::unique_ptr<chromemedia::codec::LyraEncoder> &&e,
                         std::unique_ptr<chromemedia::codec::LyraDecoder> &&d,
                         int sample_rate_hz, int num_channels, int bitrate,
                         bool enable_dtx)
    : encoder(std::move(e)),
      decoder(std::move(d)),
      sample_rate_hz(sample_rate_hz),
      num_channels(num_channels),
      bitrate(bitrate),
      enable_dtx(enable_dtx) {}

}  // namespace codec
