cmake_minimum_required(VERSION 3.16)

# === Include your custom component directories ===
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_SOURCE_DIR}/components/custom_audio
    ${CMAKE_SOURCE_DIR}/components/custom_network
    ${CMAKE_SOURCE_DIR}/components/custom_system
    ${CMAKE_SOURCE_DIR}/components/custom_utils

    # Optional: Add esp-sr and esp-dsp if you cloned them manually into components/
   ${CMAKE_SOURCE_DIR}/components/esp-sr
   ${CMAKE_SOURCE_DIR}/components/esp-dsp 
)

# Load the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake) 

# Declare the project name
project(esp32_assistant)
