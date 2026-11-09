cmake_minimum_required(VERSION 3.25)
project(Backprop LANGUAGES CXX)

# This file lives in cmake/. Source files are addressed from the project root.
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH BACKPROP_ROOT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Build configurations ---------------------------------------------------
# Picked with a CMake preset (CMakePresets.json).
#   Windows (MSVC): debug, asan, release
#   Linux (GCC or Clang): linux-debug, linux-asan, linux-release
#
#   debug    no optimization; on MSVC, a leak report when a test program exits
#   asan     debug + AddressSanitizer (out-of-bounds access, use-after-free);
#            on GCC/Clang also UndefinedBehaviorSanitizer and LeakSanitizer
#   release  optimized, for training runs

option(BACKPROP_ASAN "Build with sanitizers (ASan; plus UBSan and LSan on GCC/Clang)" OFF)

if(BACKPROP_ASAN)
  if(MSVC)
    # ASan cannot be combined with /RTC1 (part of CMake's default Debug flags)
    # or with incremental linking.
    string(REGEX REPLACE "[/-]RTC[1csu]+" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    add_compile_options(/fsanitize=address)
    add_link_options(/INCREMENTAL:NO)

    # The ASan runtime DLL sits next to the compiler. It is copied beside each
    # executable (see backprop_copy_asan_runtime) so they start from anywhere:
    # VS Code's test runner, the debugger, or a plain terminal.
    get_filename_component(_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(GLOB BACKPROP_ASAN_DLLS "${_compiler_dir}/clang_rt.asan_dynamic-*.dll")
  else()
    # ASan includes LeakSanitizer on Linux, so leaks fail the test run too.
    # -fno-sanitize-recover makes undefined behavior stop the test instead of
    # only printing a warning.
    add_compile_options(-fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  endif()
endif()

function(backprop_copy_asan_runtime target)
  if(BACKPROP_ASAN_DLLS)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BACKPROP_ASAN_DLLS} $<TARGET_FILE_DIR:${target}>
      VERBATIM)
  endif()
endfunction()

# Compiler warnings for the project's own code (not for Catch2).
function(backprop_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra)
  endif()
endfunction()

# Leak report on MSVC: only in debug. The CRT debug heap lists every allocation
# that is still alive when a test program exits. Catch2 switches the report on
# and sends it to stderr. ASan replaces the allocator, so the two cannot be
# mixed. (On Linux the asan preset's LeakSanitizer does this job.)
if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT BACKPROP_ASAN)
  set(CATCH_CONFIG_WINDOWS_CRTDBG ON CACHE BOOL "" FORCE)
else()
  set(CATCH_CONFIG_WINDOWS_CRTDBG OFF CACHE BOOL "" FORCE)
endif()

# ---- Catch2 (test framework) ------------------------------------------------
# Downloaded at configure time and compiled with the same flags as the project.

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.16.1
  GIT_SHALLOW    TRUE
  SYSTEM
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

# ---- The engine -------------------------------------------------------------
# The autograd code, compiled once and shared by the program and the tests.
# Linking to "engine" also lets a file write #include "node.h".
# Add each new engine .cpp file to this list.

add_library(engine STATIC
  "${BACKPROP_ROOT}/src/node.cpp"
  "${BACKPROP_ROOT}/src/neuron.cpp"
)
target_include_directories(engine PUBLIC "${BACKPROP_ROOT}/src")
backprop_warnings(engine)

# ---- The program ------------------------------------------------------------

add_executable(backprop
  "${BACKPROP_ROOT}/src/Main.cpp"
)
backprop_warnings(backprop)
target_link_libraries(backprop PRIVATE engine)
backprop_copy_asan_runtime(backprop)

# ---- Tests ------------------------------------------------------------------
# Add each new test file to this list.

enable_testing()

add_executable(tests
  "${BACKPROP_ROOT}/tests/example_test.cpp"
  "${BACKPROP_ROOT}/tests/backward_test.cpp"
  "${BACKPROP_ROOT}/tests/Gradient_checker_test.cpp"
  "${BACKPROP_ROOT}/tests/m2_operations_test.cpp"
)
backprop_warnings(tests)
target_link_libraries(tests PRIVATE engine Catch2::Catch2WithMain)
backprop_copy_asan_runtime(tests)

# Registers every TEST_CASE with CTest, which is what VS Code's Testing panel
# runs. A test whose output contains an MSVC leak report counts as failed.
include(Catch)
catch_discover_tests(tests
  PROPERTIES FAIL_REGULAR_EXPRESSION "Detected memory leaks"
)
