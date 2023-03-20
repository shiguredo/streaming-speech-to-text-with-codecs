## supported platforms
config_setting(
    name = "x86_64-linux",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "aarch64-macos",
    constraint_values = [
        "@platforms//os:macos",
        "@platforms//cpu:aarch64",
    ],
)

cc_library(
    name = "lyra",
    deps = [
        "@lyra//lyra:lyra_decoder",
        "@lyra//lyra:lyra_encoder",
    ],
)

## build executables
cc_binary(
    name = "main-macos",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    copts = [
        "-O3",
        "-std=c++17",
        "-pthread",
        "-Wall",
        "-I./include",
        "-D_THREAD_SAFE",
    ],
    includes = [
        "src/",
    ],
    linkopts = [
        "-framework Accelerate",
        "-framework Cocoa",
        "-framework IOKit",
        "-framework CoreAudio",
        "-framework AudioToolbox",
        "-liconv",
    ],
    deps = [
        "lyra",
        "@CLI11",
        "@opus_from_source//:opus-build",
        "@sdl2_from_source//:sdl2-build",
        "@whisper-cpp//:whisper-lib",
    ],
)

cc_binary(
    name = "main-linux",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    copts = [
        "-O3",
        "-std=c++17",
        "-I./include",
        "-pthread",
        "-Wall",
    ],
    includes = [
        "src/",
    ],
    deps = [
        "lyra",
        "@CLI11",
        "@opus_from_source//:opus-build",
        "@sdl2_from_source//:sdl2-build",
        "@whisper-cpp//:whisper-lib",
    ],
)

alias(
    name = "main",
    actual = select({
        ":x86_64-linux": "main-linux",
        ":aarch64-macos": "main-macos",
    }),
)
