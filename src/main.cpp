// Real-time speech recognition of input from a microphone
//
// A very quick-n-dirty implementation serving mainly as a proof of concept.

#include <atomic>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "CLI/CLI.hpp"
#include "audio_server.h"
#include "codec/lyra_applier.h"
#include "codec/no_codec_applier.h"
#include "codec/opus_applier.h"
#include "codec/util.h"
#include "printer.h"
#include "realtime_whisper.h"
#include "util.h"
#include "whisper.h"

///////////////////////////

void main_loop(
    const int output_key, const std::string model_path,
    const RealtimeWhisperSetting settings,
    const std::unique_ptr<codec::CodecApplierInterface> codec_applier,
    std::shared_ptr<PrinterInterface> printer,
    std::shared_ptr<AudioServer> audio_server,
    std::shared_ptr<std::atomic<bool>> is_running_shared);

int main(int argc, char **argv) {
  CLI::App app{"Whisper Realtime Example"};

  int capture_id{0};
  app.add_option("--device-id,-d", capture_id, "Audio Device id");

  std::string lyra_model_path = std::string("lyra_model_coeffs");
  app.add_option("--lyra-model", lyra_model_path, "Path to lyra models");

  // TODO: more readable option
  std::vector<WorkerParamTuple> worker_params;
  app.add_option("--worker,-w", worker_params,
                 "Worker parameter tuple(s) [n_thread, language, model_path, "
                 "codec, bitrate]");

  bool no_erase_output{false};
  app.add_flag("--no-erase-output", no_erase_output,
               "do not erase output each time");

  CLI11_PARSE(app, argc, argv);

  const int num_workers = worker_params.size();

  auto audio_server = AudioServer::Create();
  if (!audio_server->init_audio(capture_id, WHISPER_SAMPLE_RATE)) {
    std::cerr << "main: audio init error" << std::endl;
    return -1;
  }

  auto is_running = std::make_shared<std::atomic<bool>>(true);

  // init printer
  auto printer = SharedPrinter::Create(num_workers, !no_erase_output);

  std::vector<std::thread> workers;
  for (int i = 0; i < num_workers; i++) {
    const auto &p = parse_worker_param_tuple(worker_params[i]);
    auto codec_applier = codec::util::get_codec_applier(
        p.codec, p.bitrate, WHISPER_SAMPLE_RATE, lyra_model_path);
    if (codec_applier == nullptr) {
      std::cerr << "Failed to initialize #" << i << " applier." << std::endl;
      exit(EXIT_FAILURE);
    }
    const RealtimeWhisperSetting wsetting = {
        .language = p.language.c_str(),
        .n_threads = p.n_thread,
    };

    printer->set_info(i, get_param_description(p));
    workers.push_back(std::thread(main_loop, i, p.model_path, wsetting,
                                  std::move(codec_applier), printer,
                                  audio_server, is_running));
    std::cout << "hello" << std::endl;
  }

  audio_server->resume();

  for (auto &w : workers) {
    w.join();
  }

  return 0;
}

void main_loop(
    const int output_key, const std::string model_path,
    const RealtimeWhisperSetting settings,
    const std::unique_ptr<codec::CodecApplierInterface> codec_applier,
    std::shared_ptr<PrinterInterface> printer,
    std::shared_ptr<AudioServer> audio_server,
    std::shared_ptr<std::atomic<bool>> is_running_shared) {
  auto whisper = RealtimeWhisper::InitFromFile(model_path, settings);
  const int audio_key = audio_server->subscribe();

  // main audio loop
  while (true) {
    // process SDL events:

    const bool is_running_local = pollevent();
    const bool is_running_other = is_running_shared->load();
    if (!is_running_other || !is_running_local) {
      is_running_shared->store(false);
      break;
    }

    // apply codec part
    // if codec is specified to "none"
    auto pcmf32 = audio_server->get_audio_data(audio_key);
    const auto pcmf32_after_codec = codec_applier->apply(std::move(pcmf32));

    // run the inference
    {
      whisper->add_audio(pcmf32_after_codec);
      const auto infer_start = std::chrono::system_clock::now();
      if (!whisper->run_inference()) {
        std::cerr << "thread " << output_key << ": failed to process audio"
                  << std::endl;
        return;
      }
      const auto infer_end = std::chrono::system_clock::now();
      if (std::chrono::duration_cast<std::chrono::milliseconds>(infer_end -
                                                                infer_start)
              .count() < 100) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      const auto output = whisper->get_transcript(30);

      // print result;
      {
        printer->set_output(output_key, std::move(output));
        printer->print();
      }
    }
  }

  return;  // finished successfully
}