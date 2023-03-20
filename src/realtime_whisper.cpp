#include "realtime_whisper.h"

#include <memory>
#include <string>
#include <vector>

#include "whisper.h"

std::unique_ptr<RealtimeWhisper> RealtimeWhisper::InitFromFile(
    const std::string& model_path, const RealtimeWhisperSetting& _setting) {
  return std::make_unique<RealtimeWhisper>(model_path, _setting);
}

RealtimeWhisper::RealtimeWhisper(const std::string& model_path,
                                 const RealtimeWhisperSetting& _setting)
    : setting(_setting) {
  wctx = whisper_init_from_file(model_path.c_str());
}

RealtimeWhisper::~RealtimeWhisper() { whisper_free(wctx); }

void RealtimeWhisper::add_audio(const std::vector<float>& new_audio) {
  audio_buffer.insert(audio_buffer.end(), new_audio.begin(), new_audio.end());
}

bool RealtimeWhisper::run_inference() {
  // const int audio_len =
  //     std::min(30 * WHISPER_SAMPLE_RATE,
  //              std::max(0, static_cast<int>(audio_buffer.size())));
  const int audio_len = audio_buffer.size();

  if (audio_len < 1 * WHISPER_SAMPLE_RATE) {
    // if buffered audio is shorter than 1 sec, do not process it
    return true;
  }

  // setup prompt
  const int prompt_len =
      std::min(max_prompt_len, static_cast<int>(decided_tokens.size()));
  std::vector<whisper_token> prompt(prompt_len);
  for (int i = 0; i < prompt_len; i++) {
    prompt[i] = decided_tokens[decided_tokens.size() - prompt_len + i];
  }

  // run inference by whisper_full
  {
    auto params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    params.print_progress = false;
    params.print_special = true;
    params.print_realtime = false;
    params.print_timestamps = false;
    params.single_segment = true;
    params.no_context = true;  // use provide prompt

    params.translate = setting.translate;
    params.language = setting.language;

    params.temperature_inc = -1.0f;  // disable temperature fallback
    params.max_tokens = -1;          // max tokens per segments

    params.prompt_tokens = prompt.data();
    params.prompt_n_tokens = prompt_len;

    if (whisper_full(wctx, params, audio_buffer.data(), audio_len) != 0) {
      // TODO: better error handling
      return false;
    }
  }

  // pending second limit
  const float max_pending_sec = setting.max_pending_sec;

  {
    // get tokens
    const float audio_sec =
        audio_len / WHISPER_SAMPLE_RATE;  // input audio length in sec
    const float pending_start_offset = audio_sec - max_pending_sec;
    float pending_start_precise = audio_sec;

    const int num_segments = whisper_full_n_segments(wctx);
    std::vector<whisper_token> tokens, tokens_pending;
    for (int i_seg = 0; i_seg < num_segments; i_seg++) {
      const int num_token_segment = whisper_full_n_tokens(wctx, i_seg);
      const auto t_start = whisper_full_get_segment_t0(wctx, i_seg);
      const float t_start_sec = t_start / 100;

      const bool is_decided_token = t_start_sec < pending_start_offset;
      if (!is_decided_token)
        pending_start_precise = std::min(pending_start_precise, t_start_sec);

      for (int j_token = 0; j_token < num_token_segment; j_token++) {
        const auto token_id = whisper_full_get_token_id(wctx, i_seg, j_token);
        if (is_decided_token)
          tokens.push_back(token_id);
        else
          tokens_pending.push_back(token_id);
      }
    }

    {
      decided_tokens.insert(decided_tokens.end(), tokens.begin(), tokens.end());

      // put the last one into pending
      pending_tokens = std::move(tokens_pending);
    }

    {
      // update audio buffer
      const int new_start_offset =
          static_cast<int>(pending_start_precise * WHISPER_SAMPLE_RATE);
      const int new_audio_len = audio_len - new_start_offset;

      std::vector<float> new_audio(new_audio_len);
      for (int i = 0; i < new_audio_len; i++) {
        new_audio[i] = audio_buffer[new_start_offset + i];
      }

      audio_buffer = std::move(new_audio);
    }
  }

  return true;
}

std::string RealtimeWhisper::get_transcript(const int max_num_tokens) {
  const int num_tokens =
      std::min(max_num_tokens,
               static_cast<int>(pending_tokens.size() + decided_tokens.size()));

  int num_from_decided =
      std::max(0, static_cast<int>(num_tokens - pending_tokens.size()));
  const int num_from_pending = num_tokens - num_from_decided;

  {
    // search for segment start token
    const whisper_token begin_token = whisper_token_beg(wctx);
    const auto decided_token_size = decided_tokens.size();
    while (num_from_decided > 0) {
      const int idx = decided_token_size - num_from_decided;
      if (decided_tokens[idx] >= begin_token) break;
      num_from_decided -= 1;
    }
  }

  std::string text("");
  // from decided tokens
  for (int i = 0; i < num_from_decided; i++) {
    const auto token =
        decided_tokens[decided_tokens.size() - num_from_decided + i];
    if (!this->is_special_token(token))
      text += whisper_token_to_str(wctx, token);
  }

  // from pending tokens
  for (int i = 0; i < num_from_pending; i++) {
    const auto token =
        pending_tokens[pending_tokens.size() - num_from_pending + i];
    if (!this->is_special_token(token))
      text += whisper_token_to_str(wctx, token);
  }

  return text;
}

bool RealtimeWhisper::is_special_token(const whisper_token token_id) {
  const auto lang_id = whisper_lang_id(setting.language);
  return token_id == whisper_token_eot(wctx) ||
         token_id == whisper_token_sot(wctx) ||
         token_id == whisper_token_prev(wctx) ||
         token_id == whisper_token_solm(wctx) ||
         token_id == whisper_token_not(wctx) ||
         token_id >= whisper_token_beg(wctx) ||
         token_id == whisper_token_lang(wctx, lang_id);
}
