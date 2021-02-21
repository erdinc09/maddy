# This project is licensed under the MIT license. For more information see the
# LICENSE file.

cmake_minimum_required(VERSION 3.10)

include("external/cmake/HunterGate.cmake")
huntergate(URL "https://github.com/cpp-pm/hunter/archive/v0.23.279.tar.gz" SHA1 "0a8ede485c3e9c1ceed8ccb989ab6c0aba1f4db7")

project(maddy)

hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)

enable_testing()

# ------------------------------------------------------------------------------

set(MADDY_CPP_VERSION 17)
add_definitions(-DCPP_VERSION=${MADDY_CPP_VERSION})

# ------------------------------------------------------------------------------

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# ------------------------------------------------------------------------------

set(MADDY_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE MADDY_FILES ${MADDY_INCLUDE_DIR}/maddy/*.h)
file(GLOB_RECURSE MADDY_TESTS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/maddy/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/maddy/*.h )

# ------------------------------------------------------------------------------

if (UNIX)
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -g -std=c++${MADDY_CPP_VERSION} -Wall -Wpedantic -Wextra -Wno-ignored-qualifiers -fno-rtti -fno-exceptions -fsanitize=address -fno-omit-frame-pointer"
  )
else()
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++${MADDY_CPP_VERSION}"
  )
endif()

# ------------------------------------------------------------------------------

if (UNIX AND NOT APPLE)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER}
                  -fuse-ld=gold -Wl,--version
                  ERROR_QUIET OUTPUT_VARIABLE ld_version)
  if ("${ld_version}" MATCHES "GNU gold")
    message(STATUS "Found Gold linker, use faster linker")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold ")
  endif()
endif()


# ------------------------------------------------------------------------------

add_library(maddy INTERFACE ${MADDY_FILES} README.md)

target_include_directories(maddy INTERFACE
  ${MADDY_INCLUDE_DIR}
)

# ------------------------------------------------------------------------------

add_executable(
  MaddyTests
    ${MADDY_TESTS_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.cpp
)
target_include_directories(MaddyTests PUBLIC
  ${LIBS_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
target_link_libraries(MaddyTests maddy GTest::gtest GTest::gmock)
add_test(MaddyTests ${CMAKE_CURRENT_SOURCE_DIR}/build/MaddyTests)
