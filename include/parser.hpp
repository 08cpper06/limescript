#pragma once
#include <memory>
#include "tokenize.hpp"
#include "asm.hpp"
#include "types.hpp"


class ast_base_node {
public:
	virtual ~ast_base_node() = default;
	virtual std::string log(const std::string& prefix) const = 0;
	virtual void encode(asm_context& con) const = 0;
	virtual object_type type() const = 0;
	virtual ast_base_node* static_class() const = 0;
};

class ast_error_node : public ast_base_node {
public:
	~ast_error_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	std::string message;
};

class ast_value_node : public ast_base_node {
public:
	~ast_value_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	token value;
	object_type var_type;
};

class ast_parenthess_node : public ast_base_node {
public:
	~ast_parenthess_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	std::unique_ptr<ast_base_node> expr;
};

class ast_bin_op_node : public ast_base_node {
public:
	~ast_bin_op_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	token op;
	std::unique_ptr<ast_base_node> lhs;
	std::unique_ptr<ast_base_node> rhs;
};

class ast_expr_node : public ast_base_node {
public:
	~ast_expr_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	std::unique_ptr<ast_base_node> expr;
};

class ast_var_define_node : public ast_base_node {
public:
	~ast_var_define_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	token modifier;
	token var_type;
	token name;
	std::unique_ptr<ast_base_node> initial_value;
};

class ast_return_node : public ast_base_node {
public:
	~ast_return_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	std::unique_ptr<ast_base_node> expr;
};

class ast_block_node : public ast_base_node {
public:
	~ast_block_node() = default;
	std::string log(const std::string& prefix) const override;
	void encode(asm_context& con) const override;
	object_type type() const override;
	ast_base_node* static_class() const override;

public:
	std::vector<std::unique_ptr<ast_base_node>> nodes;
	std::string block_name;
};

class parser {
private:
	struct context {
		std::vector<token>::const_iterator itr;

		std::map<std::string, variable> variables;
	};
private:
	static std::unique_ptr<ast_base_node> try_parse_parenthess(context& con);
	static std::unique_ptr<ast_base_node> try_parse_value(context& con);
	static std::unique_ptr<ast_base_node> try_parse_mul_div(context& con);
	static std::unique_ptr<ast_base_node> try_parse_add_sub(context& con);
	static std::unique_ptr<ast_base_node> try_parse_return(context& con);
	static std::unique_ptr<ast_base_node> try_parse_stmt(context& con);
	static std::unique_ptr<ast_base_node> try_parse_var_define(context& con);
public:
	static std::unique_ptr<ast_base_node> parse(const std::vector<token>& tokens);
};