#include "parser.hpp"


std::string ast_error_node::log(const std::string& prefix) const {
	return prefix + "<error>" + message + "</error>\n";
}
void ast_error_node::encode(asm_context& con) const {}
object_type ast_error_node::type() const { return object_type::none; }
ast_base_node* ast_error_node::static_class() const {
	static ast_error_node instance;
	return &instance;
}

std::string ast_value_node::log(const std::string& prefix) const {
	return prefix + "<value>" + value.str + "</value>\n";
}
void ast_value_node::encode(asm_context& con) const {
	std::unique_ptr<push_instruct> inst = std::make_unique<push_instruct>();
	OBJECT object;
	switch (type()) {
	case object_type::integer:
		object = std::atoi(value.str.c_str()); 
		break;
	case object_type::floating:
		object = std::atof(value.str.c_str()); 
		break;
	default:
		object = invalid_type();
		break;
	}
	inst->value = operand {
						.type = operand_type::immidiate,
						.value = object
					};
	con.codes.push_back(std::move(inst));
}
object_type ast_value_node::type() const {
	if (value.type == token_type::number) {
		return object_type::integer;
	}
	if (value.type == token_type::floating) {
		return object_type::floating;
	}
	return object_type::none;
}
ast_base_node* ast_value_node::static_class() const {
	static ast_value_node instance;
	return &instance;
}

std::string ast_parenthess_node::log(const std::string& prefix) const {
	std::string str = prefix + "<parenthess>\n";
	if (expr) {
		str += expr->log("\t");
	}
	return str + prefix + "</parenthess>\n";
}
void ast_parenthess_node::encode(asm_context& con) const {
	if (expr) {
		expr->encode(con);
	}
}
object_type ast_parenthess_node::type() const {
	return expr ? expr->type() : object_type::none;
}
ast_base_node* ast_parenthess_node::static_class() const {
	static ast_parenthess_node instance;
	return &instance;
}

std::string ast_bin_op_node::log(const std::string& prefix) const {
	std::string str = prefix + "<operator op=\"" + op.str + ">\n";
	if (lhs) {
		str += lhs->log("\t");
	}
	if (rhs) {
		str += rhs->log("\t");
	}
	return str + prefix + "</operator>\n";
}
void ast_bin_op_node::encode(asm_context& con) const {
	if (lhs) {
		lhs->encode(con);
		if (lhs->type() != type()) {
			con.codes.push_back(std::make_unique<cast_instruct>(type()));
		}
	}
	if (rhs) {
		rhs->encode(con);
		if (rhs->type() != type()) {
			con.codes.push_back(std::make_unique<cast_instruct>(type()));
		}
	}

	if (type() == object_type::integer) {
		if (op.str == "+") {
			con.codes.push_back(std::make_unique<add_instruct>());
		} else if (op.str == "-") {
			con.codes.push_back(std::make_unique<sub_instruct>());
		} else if (op.str == "*") {
			con.codes.push_back(std::make_unique<mul_instruct>());
		} else if (op.str == "/") {
			con.codes.push_back(std::make_unique<div_instruct>());
		}
	} else if (type() == object_type::floating) {
		if (op.str == "+") {
			con.codes.push_back(std::make_unique<addf_instruct>());
		} else if (op.str == "-") {
			con.codes.push_back(std::make_unique<subf_instruct>());
		} else if (op.str == "*") {
			con.codes.push_back(std::make_unique<mulf_instruct>());
		} else if (op.str == "/") {
			con.codes.push_back(std::make_unique<divf_instruct>());
		}
	}
}
object_type ast_bin_op_node::type() const {
	object_type lhs_type = lhs ? lhs->type() : object_type::none;
	object_type rhs_type = rhs ? rhs->type() : object_type::none;
	return evaluate_type(lhs_type, rhs_type);
}
ast_base_node* ast_bin_op_node::static_class() const {
	static ast_bin_op_node instance;
	return &instance;
}

std::string ast_block_node::log(const std::string& prefix) const {
	std::string str = prefix + "<block name=\"" + block_name + "\">\n";
	for (const std::unique_ptr<ast_base_node>& node : nodes) {
		str += node->log(prefix + "\t");
	}
	return prefix + "</block>\n";
}
void ast_block_node::encode(asm_context& con) const {
	for (const std::unique_ptr<ast_base_node>& node : nodes) {
		node->encode(con);
	}
}
object_type ast_block_node::type() const {
	return object_type::none;
}
ast_base_node* ast_block_node::static_class() const {
	static ast_block_node instance;
	return &instance;
}

std::unique_ptr<ast_base_node> parser::try_parse_parenthess(context& con) {
	if (con.itr->str != "(") {
		return nullptr;
	}
	++con.itr;

	std::unique_ptr<ast_base_node> expr = parser::try_parse_add_sub(con);
	if (!expr) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "empty parenthess";
		++con.itr;
		return error;
	}
	if (con.itr->str != ")") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found `)`";
		++con.itr;
		return error;
	}
	++con.itr;
	std::unique_ptr<ast_parenthess_node> node = std::make_unique<ast_parenthess_node>();
	node->expr = std::move(expr);
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::try_parse_value(context& con) {
	std::unique_ptr<ast_base_node> expr = parser::try_parse_parenthess(con);
	if (expr) {
		return std::move(expr);
	}

	if (con.itr->type != token_type::number &&
		con.itr->type != token_type::floating) {
		return nullptr;
	}
	std::unique_ptr<ast_value_node> node = std::make_unique<ast_value_node>();
	node->value = *con.itr++;
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::try_parse_mul_div(context& con) {
	std::unique_ptr<ast_base_node> lhs = try_parse_value(con);

	while (lhs) {
		if (con.itr->str == "*" || con.itr->str == "/") {
			std::unique_ptr<ast_bin_op_node> node = std::make_unique<ast_bin_op_node>();
			node->op = *con.itr++;
			node->lhs = std::move(lhs);
			node->rhs = try_parse_value(con);
			lhs = std::move(node);
		}
		else {
			return lhs;
		}
	}
	return nullptr;
}
std::unique_ptr<ast_base_node> parser::try_parse_add_sub(context& con) {
	std::unique_ptr<ast_base_node> lhs = try_parse_mul_div(con);

	while (lhs) {
		if (con.itr->str == "+" || con.itr->str == "-") {
			std::unique_ptr<ast_bin_op_node> node = std::make_unique<ast_bin_op_node>();
			node->op = *con.itr++;
			node->lhs = std::move(lhs);
			node->rhs = try_parse_mul_div(con);
			lhs = std::move(node);
		} else {
			return lhs;
		}
	}
	return nullptr;
}
std::unique_ptr<ast_base_node> parser::try_parse_stmt(context& con) {
	if (con.itr->str == ";") {
		++con.itr;
		return try_parse_stmt(con);
	}
	std::unique_ptr<ast_base_node> node;

	node = parser::try_parse_add_sub(con);
	if (node && node->static_class() == ast_error_node().static_class()) {
		return std::move(node);
	}
	if (con.itr->str != ";" ||
		con.itr->type == token_type::eof) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found semicolon";
		return std::move(error);
	}
	++con.itr;
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::parse(const std::vector<token>& tokens) {
	context con { .itr = tokens.begin() };

	std::unique_ptr<ast_block_node> block = std::make_unique<ast_block_node>();
	block->block_name = "global";
	while (con.itr->type != token_type::eof) {
		std::unique_ptr<ast_base_node> node = try_parse_stmt(con);
		if (!node) {
			break;
		}
		block->nodes.push_back(std::move(node));
	}

	return std::move(block);
}