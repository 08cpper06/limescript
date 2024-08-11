#include "parser.hpp"


std::string ast_error_node::log(const std::string& prefix) const {
	return prefix + "<error>" + message + "</error>\n";
}
std::string ast_value_node::log(const std::string& prefix) const {
	return prefix + "<value>" + value.str + "</value>\n";
}
std::string ast_add_sub_node::log(const std::string& prefix) const {
	std::string str = prefix + "<operator op=\"" + op.str + ">\n";
	if (lhs) {
		str += lhs->log("\t");
	}
	if (rhs) {
		str += rhs->log("\t");
	}
	return str + prefix + "</operator>\n";
}

std::unique_ptr<ast_base_node> parser::try_parse_value(context& con) {
	if (con.itr->type != token_type::number) {
		return nullptr;
	}
	std::unique_ptr<ast_value_node> node = std::make_unique<ast_value_node>();
	node->value = *con.itr++;
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::try_parse_add_sub(context& con) {
	std::unique_ptr<ast_base_node> lhs = try_parse_value(con);
	if (con.itr->type != token_type::sign ||
		con.itr->str != "+") {
		return lhs;
	}
	std::unique_ptr<ast_add_sub_node> node = std::make_unique<ast_add_sub_node>();
	node->lhs = std::move(lhs);
	node->op = *con.itr++;
	node->rhs = try_parse_add_sub(con);
	if (!(node->rhs)) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found rhs";
		node->rhs = std::move(error);
	}
	return std::move(node);
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