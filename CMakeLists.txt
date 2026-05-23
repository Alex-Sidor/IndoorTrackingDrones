cmake_minimum_required(VERSION 3.15)

project(ps3eye C CXX)

set(CMAKE_CXX_STANDARD 14)

# opencv
set(WITH_IPP OFF CACHE BOOL "Disable IPP" FORCE)
set(BUILD_IPP_IW OFF CACHE BOOL "Disable IPP IW" FORCE)
set(VIDEOIO_ENABLE_PLUGINS OFF CACHE BOOL "Disable videoio plugins" FORCE)

# Explicitly disable Java to prevent space-in-path compilation crashes
set(BUILD_opencv_java OFF CACHE BOOL "Disable Java bindings" FORCE)
set(BUILD_JAVA OFF CACHE BOOL "Disable Java" FORCE)

# Enable the core modules that are needed
set(BUILD_opencv_imgcodecs ON CACHE BOOL "Required by highgui" FORCE)

# Only build the minimal modules we need to save time
set(BUILD_opencv_apps OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_calib3d OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_dnn OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_features2d OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_flann OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_gapi OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_ml OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_objdetect OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_photo OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_stitching OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_ts OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_video OFF CACHE BOOL "" FORCE)

# Now safely add the subdirectory
add_subdirectory(opencv)
add_subdirectory(imgui) 

# libusb
if(NOT TARGET libusb::usb-1.0)
    add_library(libusb::usb-1.0 UNKNOWN IMPORTED)
    
    set_target_properties(libusb::usb-1.0 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libusb-1.0.lib"
    )
endif()

# target
add_executable(ps3eye 
    src/main.cpp 
    src/ps3eye.cpp
)

target_include_directories(ps3eye PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(ps3eye PRIVATE 
    "${CMAKE_CURRENT_BINARY_DIR}"                        
    "${CMAKE_CURRENT_BINARY_DIR}/opencv"                        
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/modules/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/modules/imgproc/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/modules/imgcodecs/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/modules/highgui/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencv/modules/videoio/include"
)

# Link the targets
target_link_libraries(ps3eye 
    PRIVATE
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_highgui
    opencv_videoio
    libusb::usb-1.0
)

# move dll's and related
set(LIBUSB_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_custom_command(TARGET ps3eye POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBUSB_BIN_DIR}/libusb-1.0.dll"
        "$<TARGET_FILE_DIR:ps3eye>/"
    COMMENT "Copying libusb-1.0.dll to VS output directory..."
)

add_custom_command(TARGET ps3eye POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBUSB_BIN_DIR}/libusb-1.0.pdb"
        "$<TARGET_FILE_DIR:ps3eye>/"
    COMMENT "Copying libusb-1.0.pdb to VS output directory..."
)