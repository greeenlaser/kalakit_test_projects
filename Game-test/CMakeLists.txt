cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "game-test")
set(PROGRAM_VERSION_NUMBER 1.0.0.0)

project("game-test" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform Detection
if (WIN32)
    message(STATUS "[GAMETEST] Platform = Windows")
else()
    message(FATAL_ERROR "[GAMETEST] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Directory Setup
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

set(EXT_SHARED_DIR "${CMAKE_SOURCE_DIR}/_external_shared")

set(EXT_WINDOW_DIR "${EXT_SHARED_DIR}/Kalawindow")
set(EXT_UI_DIR "${EXT_SHARED_DIR}/KalaUI")
set(EXT_PHYSICS_DIR "${EXT_SHARED_DIR}/KalaPhysics")
set(EXT_AUDIO_DIR "${EXT_SHARED_DIR}/KalaAudio")

# Library Paths
if(IS_RELEASE)
    set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/release/KalaWindow1.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/release/KalaUI1.lib")
	set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/release/KalaPhysics1.lib")
	set(AUDIO_LIBRARY_PATH "${EXT_AUDIO_DIR}/release/KalaAudio1.lib")
else()
    set(WINDOW_LIBRARY_PATH "${EXT_WINDOW_DIR}/debug/KalaWindow1d.lib")
	set(UI_LIBRARY_PATH "${EXT_UI_DIR}/debug/KalaUI1d.lib")
	set(PHYSICS_LIBRARY_PATH "${EXT_PHYSICS_DIR}/debug/KalaPhysics1d.lib")
	set(AUDIO_LIBRARY_PATH "${EXT_AUDIO_DIR}/debug/KalaAudio1d.lib")
endif()

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*/*.cpp"
)

# Executable
add_executable(game-test ${SOURCE_FILES})

# Enable exception handling for MSVC
if (MSVC)
    target_compile_options(game-test PRIVATE /EHsc)
endif()

set_target_properties(game-test PROPERTIES OUTPUT_NAME "game-test")
target_compile_features(game-test PRIVATE cxx_std_20)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(game-test PRIVATE ${HEADERS})
target_include_directories(game-test PRIVATE
	"${INCLUDE_DIR}"
	"${EXT_SHARED_DIR}"
	"${EXT_SHARED_DIR}/KalaWindow/include"
	"${EXT_SHARED_DIR}/KalaUI/include"
	"${EXT_SHARED_DIR}/KalaPhysics/include"
	"${EXT_SHARED_DIR}/KalaAudio/include"
)

# Preprocessor Defines
target_compile_definitions(game-test PRIVATE
	WIN32_LEAN_AND_MEAN # cleaner windows.h
	NOMINMAX            # skips min/max windows macros in favor of the std variants
	UNICODE             # selects wide api variants over ansi for UTF16 windows functions
	_UNICODE            # uses wide text types in the C runtime layer
)

# Link libraries
target_link_libraries(game-test PRIVATE
	opengl32
	${WINDOW_LIBRARY_PATH}
	${UI_LIBRARY_PATH}
	${PHYSICS_LIBRARY_PATH}
	${AUDIO_LIBRARY_PATH}
)

# Hide console in release mode
# if(IS_RELEASE)
#     set_target_properties(game-test PROPERTIES WIN32_EXECUTABLE TRUE)
#     if (MSVC)
#         set_target_properties(game-test PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
#     endif()
# endif()

# Expose version string
add_compile_definitions(
	PROGRAM_VERSION="${PROGRAM_VERSION}"
)

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS game-test DESTINATION ${CMAKE_INSTALL_BINDIR})

# Copy files directory
add_custom_command(TARGET game-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:game-test>/files"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/files" "$<TARGET_FILE_DIR:game-test>/files"
)

# Copy external binaries
if(IS_RELEASE)
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/release")
	set(BIN_KALAUI "${EXT_UI_DIR}/release")
	set(BIN_KALAPHYSICS "${EXT_PHYSICS_DIR}/release")
	set(BIN_KALAAUDIO "${EXT_AUDIO_DIR}/release")
else()
	set(BIN_KALAWINDOW "${EXT_WINDOW_DIR}/debug")
	set(BIN_KALAUI "${EXT_UI_DIR}/debug")
	set(BIN_KALAPHYSICS "${EXT_PHYSICS_DIR}/debug")
	set(BIN_KALAAUDIO "${EXT_AUDIO_DIR}/debug")
endif()

if (IS_RELEASE)
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALAUI}/*.dll"
		"${BIN_KALAPHYSICS}/*.dll"
		"${BIN_KALAAUDIO}/*.dll"
	)
else()
	file(GLOB BIN_FILES
		"${BIN_KALAWINDOW}/*.dll"
		"${BIN_KALAWINDOW}/*.pdb"
		"${BIN_KALAUI}/*.dll"
		"${BIN_KALAUI}/*.pdb"
		"${BIN_KALAPHYSICS}/*.dll"
		"${BIN_KALAPHYSICS}/*.pdb"
		"${BIN_KALAAUDIO}/*.dll"
		"${BIN_KALAAUDIO}/*.pdb"
	)
endif()

set(BIN_TARGET_DIR "$<TARGET_FILE_DIR:game-test>")

foreach(BIN_FILE ${BIN_FILES})
    get_filename_component(BIN_NAME ${BIN_FILE} NAME)
    add_custom_command(TARGET game-test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
			"${BIN_FILE}"
			"${BIN_TARGET_DIR}/${BIN_NAME}"
    )
endforeach()

# Copy entire docs folder
add_custom_command(TARGET game-test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E rm -rf
		"$<TARGET_FILE_DIR:game-test>/docs"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_SOURCE_DIR}/docs"
		"$<TARGET_FILE_DIR:game-test>/docs"
)

# Package
include(CPack)
