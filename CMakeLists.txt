cmake_minimum_required(VERSION 3.14)
project(c_banking_system C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(c_banking_system main.c account.c account.h)
