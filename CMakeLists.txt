cmake_minimum_required(VERSION 3.13)
project(muon)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(muon ${SRC})
target_include_directories(muon PRIVATE "src/")
target_include_directories(muon PRIVATE "src/imgui")

# Compiler arguments
if (WIN32)
    target_compile_options(muon PRIVATE /std:c++17 /EHsc)
elseif (APPLE)
    target_compile_options(muon PRIVATE -std=c++17 -Wno-unused-command-line-argument -undefined dynamic_lookup)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(muon PRIVATE -O3)
    endif()
else()
    target_compile_options(muon PRIVATE -std=c++17)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(muon PRIVATE -O3)
    endif()
endif()

# "Universal" libraries
set(THREADS_PREFER_PTHREAD_FLAD ON)
find_package(Threads REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Libraries
if (WIN32)
    # cmake .. "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(muon PRIVATE SDL2::SDL2 SDL2::SDL2main)

    # OpenGL
    find_package(OpenGL REQUIRED)
    target_link_libraries(muon PRIVATE OpenGL::GL)
else()
    find_package(PkgConfig)

    pkg_check_modules(SDL2 REQUIRED sdl2)

    target_include_directories(muon PRIVATE
        ${SDL2_INCLUDE_DIRS}
    )

    target_link_directories(muon PRIVATE
        ${SDL2_LIBRARY_DIRS}
    )

    target_link_libraries(muon PRIVATE
        ${SDL2_LIBRARIES}
        OpenGL::GL
    )
endif()
