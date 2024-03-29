cmake_minimum_required(VERSION 3.13)
project(muon)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")

add_executable(muon ${SRC})
target_include_directories(muon PRIVATE "src/")
target_include_directories(muon PRIVATE "src/thirdparty")

# Compiler arguments
if (WIN32)
    target_compile_options(muon PRIVATE /std:c++17 /EHsc)
elseif (APPLE)
    target_compile_options(muon PRIVATE -std=c++17 -Wno-unused-command-line-argument -undefined dynamic_lookup)
    if (CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(muon PRIVATE -O3)
    endif()
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(muon PRIVATE -O1)
endif()
else()
    target_compile_options(muon PRIVATE -std=c++17)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        target_compile_options(muon PRIVATE -O1)
        target_compile_options(muon PRIVATE -Wall)
        target_compile_options(muon PRIVATE -g)
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
    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(muon PUBLIC glfw)

    # OpenGL
    target_link_libraries(muon PRIVATE OpenGL::GL)
else()
    find_package(PkgConfig)

    pkg_check_modules(GLFW3 REQUIRED glfw3)

    target_include_directories(muon PRIVATE
        ${GLFW3_INCLUDE_DIRS}
    )

    target_link_directories(muon PRIVATE
        ${GLFW3_LIBRARY_DIRS}
    )

    target_link_libraries(muon PRIVATE
        ${GLFW3_LIBRARIES}
        OpenGL::GL
    )
endif()
