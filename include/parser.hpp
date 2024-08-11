#pragma once
#include <memory>
#include "tokenize.hpp"


class ast_base_node {
public:
	virtual ~ast_base_node() = default;
	virtual std::string log(const std::string& prefix) const = 0;
};

class ast_error_node : public ast_base_node {
public:
	~ast_error_node() = default;
	std::string log(const std::string& prefix) const override;

public:
	std::string message;
};

class ast_value_node : public ast_base_node {
public:
	~ast_value_node() = default;
	std::string log(const std::string& prefix) const override;

public:
	token value;
};

class ast_add_sub_node : public ast_base_node {
public:
	~ast_add_sub_node() = default;
	std::string log(const std::string& prefix) const override;

public:
	token op;
	std::unique_ptr<ast_base_node> lhs;
	std::unique_ptr<ast_base_node> rhs;
};

class parser {
private:
	struct context {
		std::vector<token>::const_iterator itr;
	};
private:
	static std::unique_ptr<ast_base_node> try_parse_value(context& con);
	static std::unique_ptr<ast_base_node> try_parse_add_sub(context& con);
public:
	static std::unique_ptr<ast_base_node> parse(const std::vector<token>& tokens);
};