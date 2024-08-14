#pragma once
#include <string>
#include <vector>
#include <optional>


struct code_point {
	unsigned short line;
	unsigned short col;
	const char* line_head;
};

enum class token_type {
	unknown,
	number,
	floating,
	sign,
	eof,
};

struct token {
	std::string str;
	token_type type;
	code_point point;
};

class lexer {
private:
	struct context {
		code_point point;
		char* p;
	};

private:
	static void skip_space(context& con);
	static std::optional<token> try_parse_number(context& con);
	static std::optional<token> try_parse_sign(context& con);

public:
	static std::vector<token> tokenize(const std::string& sourcce);
};
