cmake_minimum_required(VERSION 3.10)

project(fuckingfs LANGUAGES C)

add_executable(fuckingfs main.c fuckingfs.c)

install(TARGETS fuckingfs RUNTIME DESTINATION bin)
