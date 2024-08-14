#include "asm.hpp"


void push_instruct::execute(asm_context& con) const {
	if (value.value.index() == INVALID_TYPE_INDEX) {
		con.codes.push_back(std::make_unique<abort_instruct>());
	}
	con.stack.push_back(value);
}
std::string push_instruct::log(const std::string& prefix) const {
	switch (value.value.index()) {
	case INT_TYPE_INDEX: /* int */
		return prefix + "push " + std::to_string(std::get<int>(value.value));
	case DOUBLE_TYPE_INDEX: /* double */
		return prefix + "push " + std::to_string(std::get<double>(value.value));
	}
	return prefix + "push none";
}

void abort_instruct::execute(asm_context& con) const {
	con.is_abort = true;
}
std::string abort_instruct::log(const std::string& prefix) const {
	return prefix + "abort\n";
}

void add_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
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
	result.value = std::get<int>(lhs.value) / std::get<int>(rhs.value);

	con.stack.push_back(std::move(result));
}
std::string div_instruct::log(const std::string& prefix) const {
	return prefix + "div\n";
}

void addf_instruct::execute(asm_context& con) const {
	operand rhs = con.stack.back(); con.stack.pop_back();
	operand lhs = con.stack.back(); con.stack.pop_back();

	operand result;
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
