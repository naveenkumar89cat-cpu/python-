cmake_minimum_required(VERSION 3.22.1)
project(robot_admin_native_host_v644)

add_library(robot_admin_native_host_v644 SHARED
    native_activity_host_v644.cpp
)

find_library(android-lib android)
find_library(log-lib log)
find_library(egl-lib EGL)
find_library(gles2-lib GLESv2)

target_compile_features(robot_admin_native_host_v644 PRIVATE cxx_std_17)
target_compile_options(robot_admin_native_host_v644 PRIVATE -O2 -Wall -Wextra)

target_link_libraries(robot_admin_native_host_v644
    ${android-lib}
    ${log-lib}
    ${egl-lib}
    ${gles2-lib}
)
