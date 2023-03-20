#include "util.h"

#include "SDL2/SDL.h"
#include "whisper.h"

bool pollevent() {
  // process SDL events:
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        return false;
      } break;
      default:
        break;
    }
  }

  return true;
}

WorkerParam parse_worker_param_tuple(const WorkerParamTuple &wptuple) {
  return WorkerParam{
      .n_thread = std::get<0>(wptuple),
      .language = std::get<1>(wptuple),
      .model_path = std::get<2>(wptuple),
      .codec = std::get<3>(wptuple),
      .bitrate = std::get<4>(wptuple),
  };
}

std::string get_param_description(const WorkerParam &wp) {
  std::string desc("");
  desc += std::to_string(wp.n_thread) + " threads";
  desc += ", ";
  desc += wp.language;
  desc += ", ";
  desc += wp.model_path;
  desc += ", ";
  desc += wp.codec;
  desc += ", ";
  desc += std::to_string(wp.bitrate) + " bps";
  return desc;
}
