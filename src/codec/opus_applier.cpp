#include "codec/opus_applier.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "opus/opus.h"

namespace codec {

OpusApplier::OpusApplier(const int32_t bitrate, const int sample_rate,
                         OpusEncoder *e, OpusDecoder *d)
    : bitrate(bitrate), sample_rate(sample_rate), encoder(e), decoder(d) {}

OpusApplier::~OpusApplier() {
  opus_encoder_destroy(encoder);
  opus_decoder_destroy(decoder);
}

std::unique_ptr<OpusApplier> OpusApplier::Create(const int32_t bitrate,
                                                 const int sample_rate) {
  constexpr int NUM_CHANNELS = 1;
  int err;
  OpusEncoder *e = opus_encoder_create(sample_rate, NUM_CHANNELS,
                                       OPUS_APPLICATION_AUDIO, &err);
  if (err < 0) return nullptr;

  err = opus_encoder_ctl(e, OPUS_SET_BITRATE(bitrate));
  if (err < 0) return nullptr;

  OpusDecoder *d = opus_decoder_create(sample_rate, NUM_CHANNELS, &err);
  if (err < 0) return nullptr;

  return std::make_unique<OpusApplier>(bitrate, sample_rate, e, d);
}

std::vector<float> OpusApplier::apply(std::vector<float> &&original) {
  constexpr int MAX_PACKET_SIZE = 3 * 1276;
  constexpr int MAX_FRAME_SIZE = 320 * 6;

  const int frame_size = sample_rate * 20 / 1000;
  unsigned char encoded_buf[MAX_PACKET_SIZE];
  std::vector<float> after_codec(original);
  int processed_count = 0;

  for (int n_consumed = 0; (n_consumed + frame_size) < original.size();
       n_consumed += frame_size) {
    // encode
    const opus_int32 pkt_size =
        opus_encode_float(encoder, original.data() + n_consumed, frame_size,
                          encoded_buf, MAX_PACKET_SIZE);
    if (pkt_size < 0) {
      // TODO: error handling
      return std::vector<float>();
    }

    const int frame_size_after = opus_decode_float(
        decoder, encoded_buf, pkt_size, after_codec.data() + processed_count,
        MAX_FRAME_SIZE, 0);

    if (frame_size_after < 0) {
      return std::vector<float>();
    }

    processed_count += frame_size_after;
  }

  after_codec.resize(processed_count);

  return after_codec;
}

}  // namespace codec
