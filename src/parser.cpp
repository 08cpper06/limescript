#include "parser.hpp"


std::string ast_error_node::log(const std::string& prefix) const {
	return prefix + "<error>" + message + "</error>\n";
}
void ast_error_node::encode(asm_context& con) const {}

std::string ast_value_node::log(const std::string& prefix) const {
	return prefix + "<value>" + value.str + "</value>\n";
}
void ast_value_node::encode(asm_context& con) const {
	std::unique_ptr<push_instruct> inst = std::make_unique<push_instruct>();
	inst->value = operand {
						.type = operand_type::immidiate,
						.value = std::atoi(value.str.c_str())
					};
	con.codes.push_back(std::move(inst));
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
	}
	if (rhs) {
		rhs->encode(con);
	}
	if (op.str == "+") {
		con.codes.push_back(std::make_unique<add_instruct>());
	} else if (op.str == "-") {
		con.codes.push_back(std::make_unique<sub_instruct>());
	} else if (op.str == "*") {
		con.codes.push_back(std::make_unique<mul_instruct>());
	} else if (op.str == "/") {
		con.codes.push_back(std::make_unique<div_instruct>());
	}
}

std::unique_ptr<ast_base_node> parser::try_parse_value(context& con) {
	if (con.itr->type != token_type::number) {
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
std::unique_ptr<ast_base_node> parser::parse(const std::vector<token>& tokens) {
	context con { .itr = tokens.begin() };
	std::unique_ptr<ast_base_node> node;

	node = try_parse_add_sub(con);

	if (con.itr->type != token_type::eof) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "invalid expression";
		return std::move(error);
	}
	return std::move(node);
}