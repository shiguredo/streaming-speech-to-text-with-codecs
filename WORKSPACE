load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

# To avoid error on apple silicon macs, we use rules_foreign_cc version 0.8.0
http_archive(
    name = "rules_foreign_cc",
    sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
    strip_prefix = "rules_foreign_cc-0.9.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/refs/tags/0.9.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

# This sets up some common toolchains for building targets. For more details, please see
# https://bazelbuild.github.io/rules_foreign_cc/0.9.0/flatten.html#rules_foreign_cc_dependencies
rules_foreign_cc_dependencies()

# load @platforms
http_archive(
    name = "platforms",
    sha256 = "5308fc1d8865406a49427ba24a9ab53087f17f5266a7aabbfc28823f3916e1ca",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
        "https://github.com/bazelbuild/platforms/releases/download/0.0.6/platforms-0.0.6.tar.gz",
    ],
)

new_git_repository(
    name = "whisper-cpp",
    build_file = "@//:whisper-cpp.BUILD",
    remote = "https://github.com/ggerganov/whisper.cpp.git",
    tag = "v1.2.1",
)

new_git_repository(
    name = "opus_from_source",
    build_file = "@//:opus.BUILD",
    remote = "https://gitlab.xiph.org/xiph/opus.git",
    tag = "v1.3.1",
)

new_git_repository(
    name = "sdl2_from_source",
    build_file = "@//:sdl2.BUILD",
    remote = "https://github.com/libsdl-org/SDL.git",
    tag = "release-2.26.3",
)

http_archive(
    name = "CLI11",
    build_file = "@//:CLI11.BUILD",
    strip_prefix = "CLI11-2.3.2",
    url = "https://github.com/CLIUtils/CLI11/archive/refs/tags/v2.3.2.tar.gz",
)

# the script below is quoted from https://github.com/shiguredo/lyra-wasm/blob/develop/wasm/WORKSPACE

#################################################################
# Lyra                                                          #
#                                                               #
# Based on https://github.com/google/lyra/blob/v1.3.0/WORKSPACE #
#################################################################
git_repository(
    name = "lyra",
    remote = "https://github.com/google/lyra.git",
    tag = "v1.3.2",
)

# proto_library, cc_proto_library, and java_proto_library rules implicitly
# depend on @com_google_protobuf for protoc and proto runtimes.
# This statement defines the @com_google_protobuf repo.
git_repository(
    name = "com_google_protobuf",
    remote = "https://github.com/protocolbuffers/protobuf.git",
    tag = "v3.20.3",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()

# Filesystem
# The new_* prefix is used because it is not a bazel project and there is
# no BUILD file in that repo.
FILESYSTEM_BUILD = """
cc_library(
  name = "filesystem",
  hdrs = glob(["include/ghc/*"]),
  visibility = ["//visibility:public"],
)
"""

new_git_repository(
    name = "gulrak_filesystem",
    build_file_content = FILESYSTEM_BUILD,
    remote = "https://github.com/gulrak/filesystem.git",
    tag = "v1.3.6",
)

# Audio DSP
git_repository(
    name = "com_google_audio_dsp",
    # There are no tags for this repo, we are synced to bleeding edge.
    commit = "14a45c5a7c965e5ef01fe537bd816ce10a247813",
    # TODO(b/231448719) use main google repo after merging PR for TF eigen compatibility.
    remote = "https://github.com/mchinen/multichannel-audio-tools.git",
    repo_mapping = {
        "@com_github_glog_glog": "@com_google_glog",
        "@eigen3": "@eigen_archive",
    },
)

# Transitive dependencies of Audio DSP.
# Note: eigen is used by Audio DSP, but provided through tensorflow workspace functions.

FFT2D_BUILD = """
# Copied from: https://github.com/google/lyra/blob/v1.3.0/external/fft2d.BUILD
package(default_visibility = ["//visibility:public"])

# Unrestricted use; can only distribute original package.
# See fft2d/readme2d.txt
licenses(["notice"])

exports_files(["LICENSE"])

# This is the main 2D FFT library.  The 2D FFTs in this library call
# 1D FFTs.  In addition, fast DCTs are provided for the special case
# of 8x8 and 16x16.  This code in this library is referred to as
# "Version II" on http://momonga.t.u-tokyo.ac.jp/~ooura/fft.html.
cc_library(
    name = "fft2d",
    srcs = [
        "fft2d/alloc.c",
        "fft2d/fftsg.c",
        "fft2d/fftsg2d.c",
        "fft2d/shrtdct.c",
    ],
    textual_hdrs = [
        "fft2d/alloc.h",
    ],
    linkopts = ["-lm"],
)
"""

http_archive(
    name = "fft2d",
    build_file_content = FFT2D_BUILD,
    sha256 = "ada7e99087c4ed477bfdf11413f2ba8db8a840ba9bbf8ac94f4f3972e2a7cec9",
    urls = [
        "http://www.kurims.kyoto-u.ac.jp/~ooura/fft2d.tgz",
    ],
)

# Google logging
git_repository(
    name = "com_google_glog",
    branch = "master",
    remote = "https://github.com/google/glog.git",
)

# Dependency for glog
git_repository(
    name = "com_github_gflags_gflags",
    branch = "android_linking_fix",
    remote = "https://github.com/mchinen/gflags.git",
)

# Bazel/build rules

http_archive(
    name = "bazel_skylib",
    sha256 = "74d544d96f4a5bb630d465ca8bbcfe231e3594e5aae57e1edbf17a6eb3ca2506",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.3.0/bazel-skylib-1.3.0.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# Begin Tensorflow WORKSPACE subset required for TFLite

git_repository(
    name = "org_tensorflow",
    # Below is reproducible and equivalent to `tag = "v2.9.0"`
    commit = "8a20d54a3c1bfa38c03ea99a2ad3c1b0a45dfa95",
    remote = "https://github.com/tensorflow/tensorflow.git",
    shallow_since = "1652465115 -0700",
)

# Check bazel version requirement, which is stricter than TensorFlow's.
load("@bazel_skylib//lib:versions.bzl", "versions")

versions.check("3.7.2")

# TF WORKSPACE Loading functions
# This section uses a subset of the tensorflow WORKSPACE loading by reusing its contents.
# There are four workspace() functions create repos for the dependencies.
# TF's loading is very complicated, and we only need a subset for TFLite.
# If we use the full TF loading sequence, we also run into conflicts and errors on some platforms.

load("@org_tensorflow//tensorflow:workspace3.bzl", "workspace")

workspace()

load("@org_tensorflow//tensorflow:workspace2.bzl", workspace2 = "workspace")

workspace2()

# quote from https://github.com/shiguredo/lyra-wasm/blob/develop/wasm/WORKSPACE end.
