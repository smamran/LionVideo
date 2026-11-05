cmake_minimum_required(VERSION 3.20)
project(LionVideo VERSION 2.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default build type to Release on single-configuration generators (Linux/GCC/Clang)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Compiler optimization flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND NOT MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffunction-sections -fdata-sections")
    if(NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,--gc-sections")
    endif()
endif()

# Auto-detect Qt6 installation on Windows if not already specified
if(WIN32 AND NOT CMAKE_PREFIX_PATH AND NOT DEFINED ENV{CMAKE_PREFIX_PATH} AND NOT DEFINED ENV{QTDIR})
    file(GLOB _qt_candidate_dirs
        "C:/Programs/Qt/6.*/llvm-mingw_64"
        "C:/Programs/Qt/6.*/mingw_64"
        "C:/Programs/Qt/6.*/msvc2022_64"
        "C:/Qt/6.*/llvm-mingw_64"
        "C:/Qt/6.*/mingw_64"
        "C:/Qt/6.*/msvc2022_64"
        "D:/Programs/Qt/6.*/llvm-mingw_64"
        "D:/Qt/6.*/llvm-mingw_64"
    )
    if(_qt_candidate_dirs)
        list(SORT _qt_candidate_dirs ORDER DESCENDING)
        list(GET _qt_candidate_dirs 0 _detected_qt)
        list(PREPEND CMAKE_PREFIX_PATH "${_detected_qt}")
        message(STATUS "Auto-detected Windows Qt6: ${_detected_qt}")
    endif()
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

# Windows-specific resources and settings
if(WIN32)
    enable_language(RC)
    set(WIN_RESOURCE resources/app.rc)
endif()

add_executable(LionVideo
    main.cpp
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${UTILS_SOURCES}
    ${RESOURCES}
    ${WIN_RESOURCE}
)

if(WIN32)
    set_target_properties(LionVideo PROPERTIES WIN32_EXECUTABLE ON)
endif()

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

# Linux / Unix installation rules and desktop integration
include(GNUInstallDirs)
install(TARGETS LionVideo
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(UNIX AND NOT APPLE)
    install(FILES resources/lionvideo.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )
    install(FILES resources/player.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
        RENAME lionvideo.png
    )
    install(FILES resources/player.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
        RENAME lionvideo.png
    )
    install(CODE "file(CREATE_LINK LionVideo \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/lionvideo\" SYMBOLIC)")
endif()

# Convenience target to run the executable cross-platform
add_custom_target(run
    COMMAND $<TARGET_FILE:LionVideo>
    DEPENDS LionVideo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Launching LionVideo..."
)

# ==============================================================================
# CPack Packaging Configuration (.deb for Debian/Ubuntu)
# ==============================================================================
set(CPACK_PACKAGE_NAME "lionvideo")
set(CPACK_PACKAGE_VENDOR "LionVideo Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern high-performance media player built with Qt 6 and C++20")
set(CPACK_PACKAGE_DESCRIPTION "Lion Media Player is a modern, high-performance media player built with Qt 6 and C++20. It features custom styled UI, playlist support, hardware-accelerated playback, volume boost, and sleek dark design.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com")
set(CPACK_PACKAGE_CONTACT "LionVideo Project <info@lionvideo.org>")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
endif()

# Debian package specific settings
set(CPACK_DEBIAN_PACKAGE_NAME "lionvideo")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LionVideo Project <info@lionvideo.org>")
set(CPACK_DEBIAN_PACKAGE_SECTION "video")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt6-qpa-plugins")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "qt6-svg-plugins")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postrm"
)

include(CPack)

if(UNIX AND NOT APPLE)
    # Convenience target to build .deb and copy to dist/
    add_custom_target(deb
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build-deb.sh
        DEPENDS LionVideo
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building Debian package (.deb) into dist/..."
    )
endif()


