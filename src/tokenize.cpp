#include "tokenize.hpp"
#include "utf8_char.hpp"


void lexer::skip_space(context& con) {
	char* pp = con.p;
	while (isspace(*pp)) {
		if (*pp == '\n') {
			++con.point.line;
			con.point.line_head = pp;
			con.point.col = -1;
		}
		++con.point.col;
		++pp;
	}
	con.p = pp;
}
std::optional<token> lexer::try_parse_number(context& con) {
	char* p = con.p;
	std::string str;
	while (*p >= '0' && *p <= '9') {
		str += *p++;
	}
	if (str.empty()) {
		if (*p != '.') {
			return std::nullopt;
		}
		str += *p++;
		if (*p < '0' || *p > '9') {
			return std::nullopt;
		}
		while (*p >= '0' && *p <= '9') {
			str += *p++;
		}
		token tok{
			.str = str,
			.type = token_type::floating,
			.point = con.point
		};
		con.point.col += p - con.p;
		con.p = p;
		return tok;
	}

	if (*p == '.') {
		str += *p++;
		while (*p >= '0' && *p <= '9') {
			str += *p++;
		}
		token tok{
			.str = str,
			.type = token_type::floating,
			.point = con.point
		};
		con.point.col += p - con.p;
		con.p = p;
		return tok;
	}
	token tok = {
		.str = str,
		.type = token_type::number,
		.point = con.point
	};
	con.point.col += p - con.p;
	con.p = p;
	return tok;
}
std::optional<token> lexer::try_parse_sign(context& con) {
	static std::vector<std::string> sign_list = {
		"+", "-", "*", "/",
		"(", ")", ";", ":", "=",
		",", "->", "{", "}"
	};

	auto start_with = [](char* p, const char* keyword) {
		while (*keyword) {
			if (*p++ != *keyword++) {
				return false;
			}
		}
		return true;
	};

	int len = 0;
	int cand_index = 0;
	for (int index = 0; index < sign_list.size(); ++index) {
		if (!start_with(con.p, sign_list[index].c_str())) {
			continue;
		}
		if (sign_list[index].size() > len) {
			len = sign_list[index].size();
			cand_index = index;
		}
	}
	if (!len) {
		return std::nullopt;
	}
	token tok = {
		.str = sign_list[cand_index],
		.type = token_type::sign,
		.point = con.point
	};
	con.p += len;
	con.point.col += len;
	return tok;
}
std::optional<token> lexer::try_parse_keyword(context& con) {
	struct keyword_info {
		std::string str;
		token_type type;
	};
	static keyword_info keywords[] = {
		{ .str = "return", .type = token_type::_return },
		{ .str = "const", .type = token_type::_const },
		{ .str = "mut", .type = token_type::_mut },
		{ .str = "int", .type = token_type::_int },
		{ .str = "float", .type = token_type::_float },
		{ .str = "fn", .type = token_type::_fn }
	};

	auto start_with = [](char* p, const char* keyword) {
		while (*keyword) {
			if (*p++ != *keyword++) {
				return false;
			}
		}
		return true;
		};

	int len = 0;
	int cand_index = 0;
	for (int index = 0; index < sizeof(keywords) / sizeof(*keywords); ++index) {
		if (!start_with(con.p, keywords[index].str.c_str())) {
			continue;
		}
		if (keywords[index].str.size() > len) {
			len = keywords[index].str.size();
			cand_index = index;
		}
	}
	if (!len) {
		return std::nullopt;
	}
	con.p += len;
	if (std::isalnum(*con.p) || *con.p == '_') {
		con.p -= len;
		return std::nullopt;
	}
	token tok = {
		.str = keywords[cand_index].str,
		.type = keywords[cand_index].type,
		.point = con.point
	};
	con.point.col += len;
	return tok;
}

std::optional<token> lexer::try_parse_identifier(context& con) {
	std::string str;
	if (!isalpha(*con.p) && *con.p != '_') {
		return std::nullopt;
	}
	str += *con.p++;
	++con.point.col;
	while (*con.p) {
		if (isalnum(*con.p) || *con.p == '_') {
			str += *con.p++;
			++con.point.col;
		}
		else {
			break;
		}
	}
	token tok = {
		.str = str,
		.type = token_type::identifier,
		.point = con.point
	};
	return tok;
}

std::vector<token> lexer::tokenize(const std::string& source) {
	context con { .point = {}, .p = const_cast<char*>(source.data()) };
	std::vector<token> tokens;

	while (*(con.p)) {
		skip_space(con);
		if (std::optional<token> tok = try_parse_number(con)) {
			tokens.push_back(tok.value());
		}
		if (std::optional<token> tok = try_parse_sign(con)) {
			tokens.push_back(tok.value());
		}
		if (std::optional<token> tok = try_parse_keyword(con)) {
			tokens.push_back(tok.value());
		}
		if (std::optional<token> tok = try_parse_identifier(con)) {
			tokens.push_back(tok.value());
		}
	}
	tokens.push_back({ .str = "", .type = token_type::eof, .point = con.point });
	return tokens;
}