cmake_minimum_required(VERSION 3.13)
project(muon)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(muon ${SRC})

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

# Libraries
if (WIN32)
    # cmake .. "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(muon PUBLIC SDL2::SDL2 SDL2::SDL2main)
else()
    find_package(PkgConfig)

    pkg_check_modules(SDL2 REQUIRED sdl2)

    target_include_directories(muon PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )

    target_link_directories(muon PUBLIC
        ${SDL2_LIBRARY_DIRS}
    )

    target_link_libraries(muon PUBLIC
        ${SDL2_LIBRARIES}
    )
endif()
