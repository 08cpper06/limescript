cmake_minimum_required(VERSION 3.8)

project(functional_test CXX)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/std:c++20 /EHsc /DWIN64")
else()
	set(CMAKE_CXX_FLAGS "-std=c++20 -Wdeprecated-declarations")
endif()

add_executable(${PROJECT_NAME}
	./src/main.cpp
	./src/functional_test_manager.cpp
	./src/functional_test.cpp

	../src/utf8_char.cpp
	../src/tokenize.cpp
	../src/parser.cpp
	../src/asm.cpp
	../src/types.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC ../include)