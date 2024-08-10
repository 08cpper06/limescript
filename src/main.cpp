#include <iostream>
#include <fstream>
#include <string>
#include "utf8_char.hpp"


int main(int argc, const char** argv) {
	if (argc == 1) {
		std::cout << "no input" << std::endl;
		return 1;
	}
	std::ifstream in(argv[1]);
	if (in.fail()) {
		std::cout << "could not found file: " << argv[1] << std::endl;
		return 2;
	}
	std::string source((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
	source.push_back('\0');

	char* p = source.data();
	utf8_char_view ch;
	do {
		ch = next(&p);
		std::cout << utf8_char(ch).data();
	} while (ch != "");

	std::cout << std::endl;

	return 0;
} 