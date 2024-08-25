#include "parser.hpp"


std::string ast_error_node::log(const std::string& prefix) const {
	if (!child) {
		return prefix + "<error>" + message + "</error>\n";
	}
	std::string str = prefix + "<error message=\"" + message + "\">\n";
	str += child->log(prefix + "\t");
	return str + prefix + "</error>\n";
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
	if (value.type == token_type::identifier) {
		inst->value = operand {
			.type = operand_type::variable,
			.value = value.str
		};
		con.codes.push_back(std::move(inst));
		return;
	}
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
	if (value.type == token_type::identifier) {
		return var_type;
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
		str += expr->log(prefix + "\t");
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
	std::string str = prefix + "<operator op=\"" + op.str + "\">\n";
	if (lhs) {
		str += lhs->log(prefix + "\t");
	}
	if (rhs) {
		str += rhs->log(prefix + "\t");
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

	if (op.str == "=") {
		ast_value_node* node = static_cast<ast_value_node*>(lhs.get());
		if (node->type() != rhs->type() &&
			evaluate_type(node->type(), rhs->type()) != object_type::none) {
			con.codes.push_back(std::make_unique<cast_instruct>(node->type()));
		}
		if (node->type() == object_type::integer) {
			std::unique_ptr<mov_instruct> mov_inst = std::make_unique<mov_instruct>();
			mov_inst->lhs = node->value.str;
			con.codes.push_back(std::move(mov_inst));
		} else if (node->type() == object_type::floating) {
			std::unique_ptr<movf_instruct> mov_inst = std::make_unique<movf_instruct>();
			mov_inst->lhs = node->value.str;
			con.codes.push_back(std::move(mov_inst));
		}
		return;
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

std::string ast_expr_node::log(const std::string& prefix) const {
	if (expr) {
		return expr->log(prefix);
	}
	return "";
}
void ast_expr_node::encode(asm_context& con) const {
	if (expr) {
		expr->encode(con);
	}
	con.codes.push_back(std::make_unique<pop_instruct>());
}
object_type ast_expr_node::type() const {
	return expr ? expr->type() : object_type::none;
}
ast_base_node* ast_expr_node::static_class() const {
	static ast_expr_node instance;
	return &instance;
}

std::string ast_var_define_node::log(const std::string& prefix) const {
	std::string type_name;
	if (modifier.type == token_type::_mut) {
		type_name = "mut ";
	} else if (modifier.type == token_type::_const) {
		type_name = "const ";
	}
	if (var_type.type == token_type::_int) {
		type_name += "int";
	} else if (var_type.type == token_type::_float) {
		type_name += "float";
	}
	std::string str = prefix + "<define name=\"" + name.str +"\" type=\"" + type_name + "\">\n";
	if (initial_value) {
		str += initial_value->log(prefix + "\t");
	}
	return str + prefix + "</define>\n";
}
void ast_var_define_node::encode(asm_context& con) const {
	std::unique_ptr<alloc_instruct> instruct = std::make_unique<alloc_instruct>();
	instruct->is_mutable = modifier.type == token_type::_mut ? true : false;
	instruct->name = name.str;
	if (var_type.type == token_type::_int) {
		instruct->type = object_type::integer;
	} else if (var_type.type == token_type::_float) {
		instruct->type = object_type::floating;
	}
	con.codes.push_back(std::move(instruct));
	if (initial_value) {
		initial_value->encode(con);
		if (initial_value->type() != type()) {
			std::unique_ptr<cast_instruct> cast_inst = std::make_unique<cast_instruct>(type());
			con.codes.push_back(std::move(cast_inst));
		}
		std::unique_ptr<init_instruct> instruct = std::make_unique<init_instruct>();
		instruct->lhs = name.str;
		con.codes.push_back(std::move(instruct));
	}
}
object_type ast_var_define_node::type() const {
	if (var_type.type == token_type::_int) {
		return object_type::integer;
	}
	if (var_type.type == token_type::_float) {
		return object_type::floating;
	}
	return object_type::none;
}
ast_base_node* ast_var_define_node::static_class() const {
	static ast_var_define_node instance;
	return &instance;
}

std::string ast_return_node::log(const std::string& prefix) const {
	std::string str = prefix + "<return>\n";
	if (expr) {
		str += expr->log(prefix + "\t");
	}
	return str + prefix + "</return>\n";
}
void ast_return_node::encode(asm_context& con) const {
	if (expr) {
		expr->encode(con);
	}
	con.codes.push_back(std::make_unique<return_instruct>());
}
object_type ast_return_node::type() const {
	return expr ? expr->type() : object_type::none;
}
ast_base_node* ast_return_node::static_class() const {
	static ast_return_node instance;
	return &instance;
}

std::string ast_block_node::log(const std::string& prefix) const {
	std::string str = prefix + "<block name=\"" + block_name + "\">\n";
	for (const std::unique_ptr<ast_base_node>& node : nodes) {
		str += node->log(prefix + "\t");
	}
	return str + prefix + "</block>\n";
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

	if (con.itr->type == token_type::identifier){
		std::unique_ptr<ast_value_node> node = std::make_unique<ast_value_node>();
		node->value = *con.itr++;
		auto itr = con.variables.find(node->value.str);
		if (itr == con.variables.end()) {
			node->var_type = object_type::none;
			std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
			error->message = "value type is not appropriate";
			error->child = std::move(node);
			return std::move(error);
		}
		node->var_type = object_type(itr->second.value.index());
		return std::move(node);
	} else if (con.itr->type != token_type::number &&
				con.itr->type != token_type::floating) {	
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "value type is not appropriate";
		return std::move(error);
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

std::unique_ptr<ast_base_node> parser::try_parse_assign(context& con) {
	std::unique_ptr<ast_base_node> lhs = parser::try_parse_add_sub(con);
	if (!lhs) {
		return nullptr;
	}
	while (lhs) {
		if (con.itr->str == "=") {
			std::unique_ptr<ast_bin_op_node> node = std::make_unique<ast_bin_op_node>();
			node->op = *con.itr++;
			node->lhs = std::move(lhs);
			node->rhs = try_parse_add_sub(con);
			if (evaluate_type(node->lhs->type(), node->rhs->type()) == object_type::none) {
				/* is castable? */
				std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
				error->message = "failed to cast " + to_string(node->rhs->type()) + " -> " + to_string(node->lhs->type());
				error->child = std::move(node);
				lhs = std::move(error);
				continue;
			} else if (node->lhs->static_class() != ast_value_node().static_class()) {
				/* is lhs assignable? */
				std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
				error->message = "assign operator's lhs is not a variable";
				error->child = std::move(node);
				lhs = std::move(error);
				continue;
			} else if (ast_value_node* value = static_cast<ast_value_node*>(node->lhs.get())) {
				auto itr = con.variables.find(value->value.str);
				if (!itr->second.is_mutable) {
					/* is lhs mutable? */
					std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
					error->message = "assign operator's lhs is not mutable";
					error->child = std::move(node);
					lhs = std::move(error);
					continue;
				}
			}
			lhs = std::move(node);
		} else {
			return lhs;
		}
	}
	return nullptr;
}

std::unique_ptr<ast_base_node> parser::try_parse_return(context& con) {
	if (con.itr->type != token_type::_return) {
		return nullptr;
	}
	++con.itr;
	std::unique_ptr<ast_base_node> expr = try_parse_add_sub(con);
	if (con.itr->str != ";") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found semicolon";
		return std::move(error);
	}
	++con.itr;
	std::unique_ptr<ast_return_node> node = std::make_unique<ast_return_node>();
	node->expr = std::move(expr);
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::try_parse_stmt(context& con) {
	if (con.itr->str == ";") {
		++con.itr;
		return try_parse_stmt(con);
	}
	std::unique_ptr<ast_base_node> node;

	node = try_parse_var_define(con);
	if (node) {
		return std::move(node);
	}

	node = parser::try_parse_return(con);
	if (node) {
		return std::move(node);
	}

	node = parser::try_parse_assign(con);
	if (node && node->static_class() == ast_error_node().static_class()) {
		return std::move(node);
	}
	if (con.itr->str != ";") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found semicolon";
		return std::move(error);
	}
	++con.itr;
	std::unique_ptr<ast_expr_node> expr = std::make_unique<ast_expr_node>();
	expr->expr = std::move(node);
	return std::move(expr);
}
std::unique_ptr<ast_base_node> parser::try_parse_var_define(context& con) {
	if (con.itr->type != token_type::_const &&
		con.itr->type != token_type::_mut) {
		return nullptr;
	}
	token modifier = *con.itr++;
	token name = *con.itr++;
	if (con.itr->str != ":") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found colon";
		return std::move(error);
	}
	++con.itr;

	if (con.itr->type != token_type::_int &&
		con.itr->type != token_type::_float) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "invalid type";
		return std::move(error);
	}
	token type = *con.itr++;
	OBJECT dummy_value;
	switch (type.type) {
	case token_type::_int:
		dummy_value = 0;
		break;
	case token_type::_float:
		dummy_value = 0.;
		break;
	}
	if (con.itr->str != ";" &&
		con.itr->str != "=") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found semicolon";
		return std::move(error);
	}

	std::unique_ptr<ast_var_define_node> node = std::make_unique<ast_var_define_node>();
	node->modifier = modifier;
	node->var_type = type;
	node->name = name;
	if (con.variables.contains(name.str)) {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = name.str + " is already defined";
		return std::move(error);
	}
	con.variables.insert({
		name.str, 
		variable {
			.name = name.str,
			.is_mutable = (modifier.type == token_type::_mut),
			.value = std::move(dummy_value)
		}
	});

	if (con.itr->str == ";") {
		++con.itr;
		return std::move(node);
	}
	++con.itr;
	std::unique_ptr<ast_base_node> initial_value = try_parse_add_sub(con);
	if (con.itr->str != ";") {
		std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
		error->message = "not found semicolon";
		return std::move(error);
	}
	++con.itr;
	node->initial_value = std::move(initial_value);
	return std::move(node);
}
std::unique_ptr<ast_base_node> parser::parse(const std::vector<token>& tokens) {
	context con { .itr = tokens.begin() };

	std::unique_ptr<ast_block_node> block = std::make_unique<ast_block_node>();
	block->block_name = "global";
	std::vector<token>::const_iterator itr;
	while (con.itr->type != token_type::eof) {
		itr = con.itr;
		std::unique_ptr<ast_base_node> node = try_parse_stmt(con);
		if (!node) {
			break;
		}
		block->nodes.push_back(std::move(node));
		if (itr == con.itr) {
			std::unique_ptr<ast_error_node> error = std::make_unique<ast_error_node>();
			error->message = "failed to parse.";
			error->child = std::move(block);
			return std::move(error);
		}
	}

	return std::move(block);
}