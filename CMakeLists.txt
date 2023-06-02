cmake_minimum_required(VERSION 3.20)
project(spectral)

include(FetchContent)
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

# find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -g -Ofast -ffast-math -Wno-gnu-zero-variadic-macro-arguments")
set(CMAKE_EXE_LINKER_FLAGS "-pthread -flto -Ofast")

# Pull in Boost libraries
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS random)
include_directories(src ${Boost_INCLUDE_DIRS})

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/materials/*.cpp" "src/scene/*.cpp" "src/util/*.cpp" "src/objects/*.cpp")
add_executable(spectral ${SOURCES})
include_directories(include)
target_link_libraries(spectral fmt::fmt-header-only ${Boost_LIBRARIES})
# target_link_libraries(OpenMP::OpenMP_CXX)
