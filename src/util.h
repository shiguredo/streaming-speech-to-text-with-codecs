#pragma once

#include <cstdint>
#include <string>
#include <thread>

struct CliParams {
  // whisper setting
  int seed = -1;
  int n_threads;
  std::string language;
  std::string model_path;

  // codec info
  std::string codec;
  int bitrate;
};

// n_thread, language, model_path, codec, bitrate
using WorkerParamTuple =
    std::tuple<int, std::string, std::string, std::string, int>;

struct WorkerParam {
  const int n_thread;
  const std::string language;
  const std::string model_path;
  const std::string codec;
  const int bitrate;
};

WorkerParam parse_worker_param_tuple(const WorkerParamTuple &wptuple);

std::string get_param_description(const WorkerParam &wp);

bool pollevent();
