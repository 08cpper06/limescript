#include <iostream>
#include <fstream>
#include <string>
#include "utf8_char.hpp"
#include "tokenize.hpp"
#include "parser.hpp"
#include "asm.hpp"


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
	std::unique_ptr<ast_base_node> node = parser::parse(tokens);
	if (!node) {
		std::cout << "failed to build AST" << std::endl;
		return 3;
	}

	asm_context con;
	node->encode(con);
	for (const std::unique_ptr<instruct>& inst : con.codes) {
		std::cout << inst->log("") << std::endl;
		inst->execute(con);
	}

	std::cout << "--------------" << std::endl;
	struct print {
		void operator()(const int& value) {
			std::cout << value << std::endl;
		}
		void operator()(const double& value) {
			std::cout << value << std::endl;
		}
		void operator()(...) {
			std::cout << "invalid type" << std::endl;
		}
	};
	std::visit(print{}, con.stack.back().value);

	return 0;
} 