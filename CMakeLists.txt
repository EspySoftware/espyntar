cmake_minimum_required(VERSION 3.11)
project(Espyntar)

set(CMAKE_C_STANDARD 99)

# Dependencies
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

add_executable(Espyntar src/main.cpp
src/components/Canvas.cpp src/headers/Canvas.h
src/components/ColorPalette.cpp src/headers/ColorPalette.h
src/components/Painter.cpp src/headers/Painter.h
src/components/ChatClient.cpp src/headers/ChatClient.h
)

target_link_libraries(Espyntar PUBLIC raylib ws2_32)
target_compile_definitions(Espyntar PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/" AUDIO_PATH="${CMAKE_CURRENT_SOURCE_DIR}/audio/")