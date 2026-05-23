cmake_minimum_required(VERSION 3.15)

project(IndoorTrackingDrones C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# opencv modules
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
set(BUILD_opencv_videoio OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_imgcodecs OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_highgui OFF CACHE BOOL "" FORCE)
set(WITH_IPP OFF CACHE BOOL "" FORCE)
set(BUILD_opencv_java OFF CACHE BOOL "" FORCE)

set(BUILD_opencv_core ON CACHE BOOL "" FORCE)
set(BUILD_opencv_imgproc ON CACHE BOOL "" FORCE)

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)

# subdirs
add_subdirectory(deps/opencv)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glm)

# libusb
if(NOT TARGET libusb::usb-1.0)
    add_library(libusb::usb-1.0 UNKNOWN IMPORTED)
    
    set_target_properties(libusb::usb-1.0 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libusb-1.0.lib"
    )
endif()

# target

file(GLOB SRC_FILES "src/*.cpp")

add_executable(IndoorTrackingDrones
    ${SRC_FILES}
    
    deps/glad/src/glad.c
    
    deps/imgui/imgui.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    
    deps/imgui/backends/imgui_impl_glfw.cpp
    deps/imgui/backends/imgui_impl_opengl3.cpp
    )

target_include_directories(IndoorTrackingDrones PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(IndoorTrackingDrones PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends"
)

target_include_directories(IndoorTrackingDrones PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include")

target_include_directories(IndoorTrackingDrones PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/opencv/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/opencv/modules/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/opencv/modules/imgproc/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/deps/opencv"
)

# Link the targets
target_link_libraries(IndoorTrackingDrones 
    PRIVATE
    opencv_core
    opencv_imgproc
    libusb::usb-1.0
    glfw
    glm
)

# move dll's
set(LUBUSB_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_custom_command(TARGET IndoorTrackingDrones POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LUBUSB_BIN_DIR}/libusb-1.0.dll"
        "$<TARGET_FILE_DIR:IndoorTrackingDrones>/"
)

add_custom_command(TARGET IndoorTrackingDrones POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LUBUSB_BIN_DIR}/libusb-1.0.pdb"
        "$<TARGET_FILE_DIR:IndoorTrackingDrones>/"
)

add_custom_command(TARGET IndoorTrackingDrones POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:glfw>"
        "$<TARGET_FILE_DIR:IndoorTrackingDrones>/"
)