load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)

cmake(
    name = "whisper-cpp-build",
    build_args = [
        "--parallel",
        "--config Release"
    ],
    generate_args = [
        "-DBUILD_SHARED_LIBS=OFF",
        "-DCMAKE_BUILD_TYPE=Release",
    ],
    lib_source = ":all_srcs",
    out_static_libs = ["static/libwhisper.a"],
)

# export as library
cc_library(
    name = "whisper-lib",
    hdrs = [
        "ggml.h",
        "whisper.h",
    ],
    includes = [
        ".",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":whisper-cpp-build",
    ],
)
