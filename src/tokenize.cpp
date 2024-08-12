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
		return std::nullopt;
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
		"+",
		"-",
		"*",
		"/"
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
	}
	tokens.push_back({ .str = "", .type = token_type::eof, .point = con.point });
	return tokens;
}