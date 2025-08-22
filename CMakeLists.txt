cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME trivex)
project(${PROJECT_NAME})

# C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# sources
file(GLOB_RECURSE SOURCES src/*.c)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(
	${PROJECT_NAME} 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Wconversion -Wshadow)
elseif (MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
endif()
