cmake_minimum_required(VERSION 3.15)

project(MotionDSP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add motion DSP source
set(MOTION_MARCO_DSP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(MOTION_MARCO_DSP_SRC "${MOTION_MARCO_DSP_ROOT}/plugins/dsp/src/dsp")
set(MOTION_MARCO_DSP_INCLUDE "${MOTION_MARCO_DSP_ROOT}/plugins/dsp/include")

# Create static library
add_library(MotionDSP STATIC

    # Wrapper files
    MotionDSP.cpp
    MotionDSP.h

    # Motion Marco DSP implementation
    ${MOTION_MARCO_DSP_SRC}/MotionPureDSP.cpp
)

# Include paths
target_include_directories(MotionDSP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MOTION_MARCO_DSP_INCLUDE}
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include/dsp"
)

# Compiler flags for iOS
target_compile_options(MotionDSP PUBLIC
    -fobjc-arc
    -fembed-bitcode
)

# Architecture settings for iOS
if(IOS_PLATFORM)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0")
endif()
