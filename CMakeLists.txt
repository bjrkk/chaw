cmake_minimum_required(VERSION 3.0.0)
project(chomp VERSION 0.1.0)

include(CTest)
enable_testing()

if (WIN32)
	add_compile_definitions ("_CRT_SECURE_NO_WARNINGS")
endif ()

set (CMAKE_C_STANDARD 23)

add_executable(chomp 
	main.c 
	compile.c
	util.c
	ext/vec.c
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
