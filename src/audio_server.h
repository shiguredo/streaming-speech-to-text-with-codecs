#pragma once

#include <memory>
#include <mutex>
#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"

class AudioServer {
 public:
  static std::shared_ptr<AudioServer> Create();

  AudioServer();
  ~AudioServer();

  bool init_audio(const int capture_id, const int sample_rate);

  int subscribe();

  void set_data_to_buffer(uint8_t* stream, int len);

  std::vector<float> get_audio_data(const int sample_len);

  bool resume();

 private:
  bool is_initialized = false;
  SDL_AudioDeviceID device_id;

  std::mutex mtx;

  const int buffered_sec = 60;  // default buffered data is 60 sec
  int buffer_size;
  std::vector<float> audio_buffer;

  int buffered_audio_start_offset;
  int buffered_audio_newest_offset;

  std::vector<int> start_offsets;
};
