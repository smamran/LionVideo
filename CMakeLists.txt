cmake_minimum_required(VERSION 3.20)
project(LionVideo VERSION 2.0.0 LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Compiler optimization flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
endif()

# Find required Qt6 packages
find_package(Qt6 COMPONENTS
    Widgets
    Multimedia
    MultimediaWidgets
    Svg
    REQUIRED
)

# Project sources
set(CORE_SOURCES
    src/core/MediaPlayerEngine.h
    src/core/MediaPlayerEngine.cpp
)

set(UI_SOURCES
    src/ui/ClickableSlider.h
    src/ui/ClickableSlider.cpp
    src/ui/ControlBar.h
    src/ui/ControlBar.cpp
    src/ui/OSDLabel.h
    src/ui/OSDLabel.cpp
    src/ui/PlaylistDrawer.h
    src/ui/PlaylistDrawer.cpp
    src/ui/TitleBar.h
    src/ui/TitleBar.cpp
    src/ui/VideoWidget.h
    src/ui/VideoWidget.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
)

set(UTILS_SOURCES
    src/utils/IconHelper.h
    src/utils/IconHelper.cpp
    src/utils/StyleManager.h
    src/utils/StyleManager.cpp
    src/utils/TimeFormatter.h
    src/utils/TimeFormatter.cpp
)

set(RESOURCES
    resources/resources.qrc
)

set(WIN_RESOURCE
    resources/app.rc
)

add_executable(LionVideo WIN32
    main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${UTILS_SOURCES}
    ${RESOURCES}
    ${WIN_RESOURCE}
)

target_include_directories(LionVideo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(LionVideo PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Svg
)

if(WIN32)
    target_link_libraries(LionVideo PRIVATE
        user32
        gdi32
        shell32
    )
endif()