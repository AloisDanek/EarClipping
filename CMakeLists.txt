cmake_minimum_required(VERSION 3.14)
project(geometry_ear_clipping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ear_clip
  main.cpp
  Bounds.cpp
  GeometryTypes.cpp
  KDTree.cpp
  Point.cpp
  Triangle.cpp
  Polygon.cpp
  EarClipping.cpp
  CsvInput.cpp
  CsvOutput.cpp
  SvgOutput.cpp
  UnitTests.cpp
)

target_compile_options(ear_clip PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)
