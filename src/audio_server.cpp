#include "audio_server.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"

std::shared_ptr<AudioServer> AudioServer::Create() {
  return std::make_unique<AudioServer>();
}

AudioServer::AudioServer() : device_id(0), is_initialized(false) {}

AudioServer::~AudioServer() {}

int AudioServer::subscribe() {
  std::lock_guard<std::mutex> _lock(mtx);
  const int new_key = start_offsets.size();
  start_offsets.push_back(0);

  return new_key;
}

bool AudioServer::init_audio(const int capture_id, const int sample_rate) {
  if (is_initialized) {
    std::cerr << "SDL: audio already initialized" << std::endl;
    return false;
  }

  SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s\n",
                 SDL_GetError());
    return false;
  }

  SDL_SetHintWithPriority(SDL_HINT_AUDIO_RESAMPLING_MODE, "medium",
                          SDL_HINT_OVERRIDE);
  {
    // list up all available audio devices
    int num_devices = SDL_GetNumAudioDevices(SDL_TRUE);
    std::cerr << "SDL: found " << num_devices << " capture devices."
              << std::endl;
    for (int i = 0; i < num_devices; i++) {
      std::cerr << "SDL:    - device #" << i << ": '"
                << SDL_GetAudioDeviceName(i, SDL_TRUE) << "'" << std::endl;
    }
  }

  {
    // set up audio device
    SDL_AudioSpec capture_spec_requested;
    SDL_AudioSpec capture_spec_obtained;

    SDL_zero(capture_spec_requested);
    SDL_zero(capture_spec_obtained);

    {
      // capture spec
      capture_spec_requested.freq = sample_rate;
      capture_spec_requested.format = AUDIO_F32;
      capture_spec_requested.channels = 1;
      capture_spec_requested.samples = 1024;
      capture_spec_requested.callback = [](void *userdata, uint8_t *stream,
                                           int len) {
        auto audio_server = static_cast<AudioServer *>(userdata);
        audio_server->set_data_to_buffer(stream, len);
      };
      capture_spec_requested.userdata = this;
    }

    if (capture_id >= 0) {
      std::cerr << "SDL: attempt to open capture device #" << capture_id
                << std::endl;
      device_id =
          SDL_OpenAudioDevice(SDL_GetAudioDeviceName(capture_id, SDL_TRUE),
                              SDL_TRUE, &capture_spec_requested,
                              &capture_spec_obtained, /* allowed_changes */ 0);
    } else {
      std::cerr << "SDL: audio device id must be 0 or greater." << std::endl;
      return false;
    }

    // error handling for device capture failure
    if (!device_id) {
      std::cerr << "SDL: failed to capture audio device." << std::endl;
      device_id = 0;
      return false;
    }

    // TODO: logging obtained audio spec
  }

  // setup buffer
  {
    buffer_size = sample_rate * buffered_sec;  // buffering 60 sec data
    audio_buffer.resize(buffer_size);

    buffered_audio_start_offset = 0;
    buffered_audio_newest_offset = 0;
  }

  is_initialized = true;
  return true;
}

void AudioServer::set_data_to_buffer(uint8_t *stream, int len) {
  const size_t n_samples = len / sizeof(float);

  std::vector<float> audio_stream(n_samples);
  memcpy(audio_stream.data(), stream, n_samples * sizeof(float));

  {
    // set data to simple ring buffer
    std::lock_guard<std::mutex> _lock(mtx);

    const int audio_last_index = buffered_audio_newest_offset % buffer_size;
    for (int i = 0; i < n_samples; i++) {
      const int buffer_index = (audio_last_index + i) % buffer_size;
      audio_buffer[buffer_index] = audio_stream[i];
    }

    buffered_audio_newest_offset += n_samples;
    if (buffered_audio_start_offset + buffer_size <
        buffered_audio_newest_offset) {
      buffered_audio_start_offset = buffered_audio_newest_offset - buffer_size;
    }
  }
}

bool AudioServer::resume() {
  if (!is_initialized) {
    return false;
  }
  SDL_PauseAudioDevice(device_id, 0);
  return true;
}

std::vector<float> AudioServer::get_audio_data(const int key) {
  std::lock_guard<std::mutex> _lock(mtx);

  const int start_offset =
      std::max(buffered_audio_start_offset, start_offsets[key]);
  const int sample_len_available =
      std::max(0, buffered_audio_newest_offset - start_offset);
  std::vector<float> output(sample_len_available);

  for (int i = 0; i < sample_len_available; i++) {
    const int buffer_index = (start_offset + i) % buffer_size;
    output[i] = audio_buffer[buffer_index];
  }

  start_offsets[key] = start_offset + sample_len_available;

  return output;
}
