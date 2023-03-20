# streaming speech to text with codecs

## About Shiguredo's open source software

We will not respond to PRs or issues that have not been discussed on Discord. Also, Discord is only available in Japanese.

Please read https://github.com/shiguredo/oss before use.

## 時雨堂のオープンソースソフトウェアについて

利用前に https://github.com/shiguredo/oss をお読みください。

## 概要

[Opus](https://opus-codec.org/) や [lyra](https://github.com/google/lyra) などの音声圧縮コーデックを経由した音声を用いてリアルタイムに音声認識を行うための example です。

## Requirements

Linux, macOS (Apple Silicon) 上でのビルドに対応しています。Windows にはネイティブでの実行には対応していませんが、WSL2 経由で利用できることを確認しています。

Linux については Ubuntu 22.04 上でビルドができることを確認しています。

ビルドのためには 5 系の[Bazel](https://bazel.build/)が必要です。（lyra のビルドが bazel v6 では失敗するため）

[Bazelisk](https://github.com/bazelbuild/bazelisk)経由で Bazel を利用すると自動でバージョンの問題を解決してくれるので Bazelisk を利用することをおすすめします。こちらは`go install`のコマンドで簡単にインストールできます。

## ビルド・実行手順

Bazelisk を利用している場合は`bazel`コマンドを適宜`bazelisk`コマンドに読み替えてください。

次のコマンドでビルドできます。

```bash
bazel build //:main
```

実行は次のコマンドです。

```bash
# Linux
bazel-bin/main-linux [OPTIONS]

# MacOS
bazel-bin/main-macos [OPTIONS]
```

### whisper.cpp モデルのダウンロード

以下のコマンドでダウンロードできます。

```
models/download-ggml-model.sh model_name
```

詳しくは[こちら](models/README.md)を参照してください。

## オプションについて

例えば、次のようなコマンドで実行できます。

```bash
bazel-bin/main-linux -w 3 ja path/to/models/ggml-base.bin none 0  -w 3 ja path/to/models/ggml-base.bin lyra 3200
```

### `-w,--worker`

文字起こしを実行する worker を追加するためのオプションです。worker で用いる各種パラメータを指定する必要があります。

```
-w [number of thread] [language] [path to whisper model] [codec] [codec bitrate]
```

#### param1: number of thread

worker が利用するスレッド数を指定します。1worker あたり 4 スレッド程度割り当てることをおすすめします。

#### param2: language

文字起こしの言語を指定します。指定方法は`whisper.cpp`と同様です。

#### param3: path to whisper model

利用する whisper.cpp のモデルを指定します。

#### param4: codec

適用するコーデックを指定します。以下のコーデックが指定できます。

- `none`
  - 何も適用しない
- `lyra`
- `opus`

#### param5: codec bitrate

コーデックのビットレート(bps)を指定します。コーデックに`none`が指定されている場合、この値は無視されます。

- lyra の対応ビットレート
  - `3200, 6000, 9200`
- Opus の対応ビットレート
  - `6000 ~ 510000`

### `-d,--device-id`

マイクデバイス ID を指定します。デフォルトは 0 です。

### `--lyra-model`

コーデックに`lyra`を指定したときに利用するモデルのパスの指定です。
デフォルトでは`lyra_model_coeffs/`となっています。

### `--no-erase-output`

新たな出力を表示するときに、前の出力を消去せずに表示します。

## パフォーマンスについて

このプログラムでは whisper.cpp を利用し、CPU で推論を行っています。1 worker あたり 4 スレッドを割り当てることで、base モデルでおよそ 1 秒程度の遅延で文字起こしをすることができます。

Whisper のモデルはリアルタイムに文字起こしをするためには制約が厳しい（入力が 30 秒の固定長音声であるなど）ため、多くのリアルタイム音声認識よりも遅延が大きくなってしまっています。Whisper や他の音声認識モデルを用いてより小さな遅延で音声認識を行う方法を模索中です。

## LICENSE

```
Copyright 2023-2023, so298 (Original Author)
Copyright 2023-2023, Shiguredo Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
