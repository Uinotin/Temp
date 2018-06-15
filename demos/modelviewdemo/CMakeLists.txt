cmake_minimum_required(VERSION 3.2)
add_executable(modelviewdemo main.c modelviewmodule.c)
target_link_libraries(modelviewdemo engine)
target_include_directories(modelviewdemo PRIVATE ../../engine)
