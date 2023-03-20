#pragma once

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "whisper.h"

struct RealtimeWhisperSetting {
  static const int default_audio_sec = 30;

  const char* language = "en";
  int n_threads =
      std::min(4, static_cast<int>(std::thread::hardware_concurrency()));
  bool translate = false;
  const int buf_size = WHISPER_SAMPLE_RATE * default_audio_sec;
  const float max_pending_sec = 10;
};

class RealtimeWhisper {
 public:
  static std::unique_ptr<RealtimeWhisper> InitFromFile(
      const std::string& model_path, const RealtimeWhisperSetting& _setting);

  RealtimeWhisper(const std::string& model_path,
                  const RealtimeWhisperSetting& _setting);
  ~RealtimeWhisper();

  void add_audio(const std::vector<float>& audio);
  bool run_inference();
  std::string get_transcript(const int max_num_tokens);

 private:
  bool is_special_token(const whisper_token token_id);

  whisper_context* wctx;
  std::vector<float> audio_buffer;
  const RealtimeWhisperSetting setting;

  int decided_audio_idx = 0;
  int end_audio_idx = 0;

  static const int max_prompt_len = 128;

  std::vector<whisper_token> decided_tokens;
  std::vector<whisper_token> pending_tokens;
};
