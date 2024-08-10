#include <iostream>
#include <fstream>
#include <string>
#include "utf8_char.hpp"
#include "tokenize.hpp"


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

	std::vector<token> tokens = lexer::tokenize(source);
	for (const token& tok : tokens) {
		std::cout << tok.str << std::endl;
	}

	return 0;
} 