cmake_minimum_required(VERSION 3.10)
project(LedController)
set(LIBRARY_OUTPUT_NAME ledcontroller)

# Set the JNI home directory
set(JNI_HOME /lib/jvm/java-11-openjdk-amd64/)

# Include directories for JNI
include_directories(${JNI_HOME}/include)
include_directories(${JNI_HOME}/include/linux)
include_directories(include)


# Source file
set(SOURCE_FILE jni/LedController.cpp leds/leds.c)

# Add the library
add_library(${LIBRARY_OUTPUT_NAME} SHARED ${SOURCE_FILE})

target_link_libraries(${LIBRARY_OUTPUT_NAME} ${CMAKE_DL_LIBS})

set_target_properties(${LIBRARY_OUTPUT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
