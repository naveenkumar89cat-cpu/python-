cmake_minimum_required(VERSION 3.22.1)
project(robot_admin_native_host_v643)

add_library(robot_admin_native_host_v643 SHARED
    native_activity_host_v643.cpp
)

find_library(android-lib android)
find_library(log-lib log)

target_compile_features(robot_admin_native_host_v643 PRIVATE cxx_std_17)
target_compile_options(robot_admin_native_host_v643 PRIVATE -O2 -Wall -Wextra)

target_link_libraries(robot_admin_native_host_v643
    ${android-lib}
    ${log-lib}
)
