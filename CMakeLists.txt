cmake_minimum_required(VERSION 3.8)

project(limescript CXX)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/std:c++20 /EHsc /DWIN64")
else()
	set(CMAKE_CXX_FLAGS "-std=c++20 -Wdeprecated-declarations")
endif()

add_executable(${PROJECT_NAME}
	./src/main.cpp
	./src/utf8_char.cpp
	./src/tokenize.cpp
	./src/parser.cpp
	./src/asm.cpp
	./src/types.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)

add_subdirectory(functional_test)