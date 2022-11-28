# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.

project("zplayer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -frtti")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -ffunction-sections -fdata-sections -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")


set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Os  -Wl,--exclude-libs,ALL -Wl,--gc-sections,-s -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/version_script.txt")


file(GLOB native_file src/main/cpp/*.c src/main/cpp/*.cpp)

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)

include_directories(src/main/cpp/libyuv/include)
add_subdirectory(src/main/cpp/libyuv)

add_library(zplayer

        SHARED

        ${native_file})


find_library(
        log-lib
        log)

add_library(avutil
        SHARED
        IMPORTED)

set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.so)


add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.so)

add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavdevice.so)

add_library(postproc
        SHARED
        IMPORTED)
set_target_properties(postproc
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libpostproc.so)

include_directories(${JNI_LIBS_DIR}/include/ffmpeg/${ANDROID_ABI})
target_link_libraries(
        zplayer
        avdevice
        avutil
        swresample
        swscale
        avcodec
        avformat
        avfilter
        yuv
        postproc
        android
        OpenSLES
        ${log-lib})