cmake_minimum_required(VERSION 3.16)
project(LionVideo)

set(CMAKE_BUILD_TYPE Release)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Optimize for size and strip symbols
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -flto -ffunction-sections -fdata-sections -fno-exceptions")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections -Wl,-s")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt6 Packages required for multimedia
find_package(Qt6 COMPONENTS Widgets Multimedia MultimediaWidgets REQUIRED)

add_executable(LionVideo WIN32 main.cpp)

target_link_libraries(LionVideo PRIVATE
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)