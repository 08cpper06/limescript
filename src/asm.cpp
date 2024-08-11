#include "asm.hpp"


void push_instruct::execute(asm_context& con) const {
	con.stack.push_back(value);
}
std::string push_instruct::log(const std::string& prefix) const {
	return prefix + "push " + std::to_string(std::get<int>(value.value));
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
