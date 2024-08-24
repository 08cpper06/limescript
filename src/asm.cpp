#include "asm.hpp"
#include <iostream>


void push_instruct::execute(asm_context& con) const {
	if (value.value.index() == INVALID_TYPE_INDEX) {
		con.codes.push_back(std::make_unique<abort_instruct>());
	}
	if (value.type == operand_type::variable) {
		std::string var_name = std::get<std::string>(value.value);
		con.stack.push_back(operand {
			.type = operand_type::immidiate,
			.value = con.variables.find(var_name)->second.value
		});
		return;
	}
	con.stack.push_back(value);
}
std::string push_instruct::log(const std::string& prefix) const {
	switch (value.value.index()) {
	case INT_TYPE_INDEX: /* int */
		return prefix + "push " + std::to_string(std::get<int>(value.value));
	case DOUBLE_TYPE_INDEX: /* double */
		return prefix + "push " + std::to_string(std::get<double>(value.value));
	case STRING_TYPE_INDEX:
		if (value.type == operand_type::variable) {
			return prefix + "push " + std::get<std::string>(value.value);
		}
	}
	return prefix + "push none";
}

void pop_instruct::execute(asm_context& con) const {
	con.stack.pop_back();
}
std::string pop_instruct::log(const std::string& prefix) const {
	return prefix + "pop";
}

void alloc_instruct::execute(asm_context& con) const {
	OBJECT value;
	if (type == object_type::integer) { value = 0; }
	else if (type == object_type::floating) { value = 0.; }
	if (con.variables.contains(name)) {
		std::cout << name << "is already defined" << std::endl;
		con.is_abort = true;
		return;
	}
	con.variables.insert({
		name,
		variable {
			.name = name,
			.is_mutable = is_mutable,
			.value = value
		}
	});
}
std::string alloc_instruct::log(const std::string& prefix) const {
	std::string type_name;
	if (type == object_type::integer) {
		type_name = "int";
	} else if (type == object_type::floating) {
		type_name = "float";
	}
	return prefix + "alloc " + type_name + (is_mutable ? " const" : " mut") + " as " + name;
}

void init_instruct::execute(asm_context& con) const {
	operand value = con.stack.back();
	con.stack.pop_back();
	auto itr = con.variables.find(lhs);
	if (itr == con.variables.end()) {
		std::cout << "not found variable: " << lhs << std::endl;
		con.is_abort = true;
		return;
	}
	if (itr->second.is_init) {
		std::cout << lhs << " is already initialized" << std::endl;
		con.is_abort = true;
		return;
	}
	itr->second.value = value.value;
	itr->second.is_init = true;
}
std::string init_instruct::log(const std::string& prefix) const {
	std::string str = prefix + "init ";
	return str + lhs;
}

void return_instruct::execute(asm_context& con) const {
	con.is_abort = true;
}
std::string return_instruct::log(const std::string& prefix) const {
	return prefix + "return";
}

void abort_instruct::execute(asm_context& con) const {
	con.is_abort = true;
}
std::string abort_instruct::log(const std::string& prefix) const {
	return prefix + "abort\n";
}

void mov_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	auto itr = con.variables.find(lhs);
	if (itr == con.variables.end()) {
		std::cout << "not found variable: " << lhs << std::endl;
		con.is_abort = true;
		return;
	}
	if (!(itr->second.is_mutable)) {
		std::cout << lhs << "is immutable" << std::endl;
		con.is_abort = true;
		return;
	}
	if (itr->second.value.index() != rhs.value.index()) {
		std::cout << "mov is failed (reason: defferent type)" << std::endl;
		con.is_abort = true;
		return;
	}
	itr->second.value = rhs.value;
}
std::string mov_instruct::log(const std::string& prefix) const {
	return prefix + "mov " + lhs + "\n";
}

void add_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<int>(lhs.value) + std::get<int>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string add_instruct::log(const std::string& prefix) const {
	return prefix + "add\n";
}

void sub_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<int>(lhs.value) - std::get<int>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string sub_instruct::log(const std::string& prefix) const {
	return prefix + "sub\n";
}

void mul_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<int>(lhs.value) * std::get<int>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string mul_instruct::log(const std::string& prefix) const {
	return prefix + "mul\n";
}

void div_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<int>(lhs.value) / std::get<int>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string div_instruct::log(const std::string& prefix) const {
	return prefix + "div\n";
}

void movf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	auto itr = con.variables.find(lhs);
	if (itr == con.variables.end()) {
		std::cout << "not found variable: " << lhs << std::endl;
		con.is_abort = true;
		return;
	}
	if (!(itr->second.is_mutable)) {
		std::cout << lhs << "is immutable" << std::endl;
		con.is_abort = true;
		return;
	}
	if (itr->second.value.index() != rhs.value.index()) {
		std::cout << "mov is failed (reason: defferent type)" << std::endl;
		con.is_abort = true;
		return;
	}
	itr->second.value = rhs.value;
}
std::string movf_instruct::log(const std::string& prefix) const {
	return prefix + "movf " + lhs + "\n";
}

void addf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<double>(lhs.value) + std::get<double>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string addf_instruct::log(const std::string& prefix) const {
	return prefix + "addf";
}

void subf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<double>(lhs.value) - std::get<double>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string subf_instruct::log(const std::string& prefix) const {
	return prefix + "subf";
}

void mulf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<double>(lhs.value) * std::get<double>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string mulf_instruct::log(const std::string& prefix) const {
	return prefix + "mulf";
}

void divf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
	result.type = operand_type::immidiate;
	result.value = std::get<double>(lhs.value) / std::get<double>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string divf_instruct::log(const std::string& prefix) const {
	return prefix + "divf";
}

cast_instruct::cast_instruct(object_type type) :
	to(type)
{}
void cast_instruct::execute(asm_context& con) const {
	struct cast_to {
		object_type to;
		OBJECT operator()(const invalid_type&) { return invalid_type(); }
		OBJECT operator()(const int& value) {
			switch (to) {
			case object_type::none: return invalid_type();
			case object_type::integer: return value;
			case object_type::floating: return static_cast<double>(value);
			}
			return invalid_type();
		}
		OBJECT operator()(const double& value) {
			switch (to) {
			case object_type::none: return invalid_type();
			case object_type::integer: return static_cast<int>(value);
			case object_type::floating: return value;
			}
			return invalid_type();
		}
		OBJECT operator()(...) { return invalid_type(); }
	};
	operand object = con.stack.back(); con.stack.pop_back();
	cast_to visitor { .to = to };
	con.stack.push_back(operand{ .type = operand_type::immidiate, .value = std::visit(visitor, object.value) });
	if (con.stack.back().value.index() == INVALID_TYPE_INDEX) {
		con.codes.push_back(std::make_unique<abort_instruct>());
	}
}
std::string cast_instruct::log(const std::string& prefix) const {
	std::string str = prefix + "cast ";
	switch (to) {
	case object_type::none:
		str += "none\n";
		break;
	case object_type::integer:
		str += "int\n";
		break;
	case object_type::floating:
		str += "double\n";
		break;
	}
	return str;
}
