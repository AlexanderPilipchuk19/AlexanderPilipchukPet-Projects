cmake_minimum_required(VERSION 3.16)
project(fft_signal_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(dsp_lab
    src/main.cpp
    src/cli.cpp
    src/signal.cpp
    src/generator.cpp
    src/fft.cpp
    src/window.cpp
    src/spectrum.cpp
    src/reconstruction.cpp
)

target_include_directories(dsp_lab PRIVATE include)

target_compile_options(dsp_lab PRIVATE -Wall -Wextra -Wpedantic)
