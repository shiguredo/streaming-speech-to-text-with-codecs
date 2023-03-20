load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)

cmake(
    name = "sdl2-build",
    build_args = [
        "--config Release",
        "--parallel",
    ],
    generate_args = [
        "-DCMAKE_BUILD_TYPE=Release",
        "-DSDL_VIDEO=OFF",
        "-DSDL_HAPTIC=OFF",
        "-DSDL_JOYSTICK=OFF",
        "-DSDL_HIDAPI=OFF",
        "-DSDL_RENDER=OFF",
    ],
    lib_source = ":all_srcs",
    out_static_libs = ["libSDL2.a"],
    visibility = ["//visibility:public"],
)
