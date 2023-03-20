load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
)

cmake(
    name = "opus-build",
    build_args = [
        "--parallel",
    ],
    generate_args = [
        "-DCMAKE_BUILD_TYPE=Release",
    ],
    lib_source = ":all_srcs",
    out_static_libs = ["libopus.a"],
    visibility = ["//visibility:public"],
)
