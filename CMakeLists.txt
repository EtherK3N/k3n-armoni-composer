cmake_minimum_required(VERSION 3.22)
project(LoopStation VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# JUCE Resolution: Subdirectory -> Installed Package -> FetchContent Auto-Download
# ---------------------------------------------------------------------------
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/CMakeLists.txt")
    add_subdirectory(JUCE)
else()
    find_package(JUCE CONFIG QUIET)
    if (NOT JUCE_FOUND)
        message(STATUS "JUCE not found locally. Automatically downloading via FetchContent...")
        include(FetchContent)
        FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 7.0.12
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(JUCE)
    endif()
endif()


# ===========================================================================
# Main GUI Application
# ===========================================================================
juce_add_gui_app(LoopStation
    PRODUCT_NAME "LoopStation"
    COMPANY_NAME "k3n"
    VERSION 0.1.0
    NEEDS_JUCE_HEADER TRUE
)

target_sources(LoopStation PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/DeviceManager.cpp
    Source/RawInputHandler.cpp
    Source/MappingEngine.cpp
    Source/AudioEngine.cpp
    Source/MetronomeClock.cpp
    Source/BpmQuantizer.cpp
    Source/MappingEditorComponent.cpp
    Source/PerformanceViewComponent.cpp
)

target_include_directories(LoopStation PRIVATE
    Source
)

target_compile_definitions(LoopStation PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(LoopStation PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
)

if (WIN32)
    target_link_libraries(LoopStation PRIVATE hid.lib setupapi.lib user32.lib wininet.lib)
endif()


# ===========================================================================
# Automated Unit Test Suite
# ===========================================================================
juce_add_console_app(LoopStation_Tests
    PRODUCT_NAME "LoopStation_Tests"
    NEEDS_JUCE_HEADER TRUE
)

target_sources(LoopStation_Tests PRIVATE
    Tests/TestRunner.cpp
    Source/DeviceManager.cpp
    Source/RawInputHandler.cpp
    Source/MappingEngine.cpp
    Source/AudioEngine.cpp
    Source/MetronomeClock.cpp
    Source/BpmQuantizer.cpp
)

target_include_directories(LoopStation_Tests PRIVATE
    Source
)

target_compile_definitions(LoopStation_Tests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(LoopStation_Tests PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
)

if (WIN32)
    target_link_libraries(LoopStation_Tests PRIVATE hid.lib setupapi.lib user32.lib wininet.lib)
endif()

