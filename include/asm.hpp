#pragma once
#include <list>
#include <memory>
#include <variant>
#include <string>


class instruct;
class operand;

struct asm_context {
	std::list<operand> stack;

	std::list<std::unique_ptr<instruct>> codes;
};

class instruct {
public:
	virtual ~instruct() = default;
	virtual void execute(asm_context& con) const = 0;
	virtual std::string log(const std::string& prefix) const = 0;
};

enum class operand_type {
	immidiate,
};

struct operand {
	operand_type type;
	std::variant<int> value;
};

class push_instruct : public instruct {
public:
	~push_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	operand value;
};

class add_instruct : public instruct {
public:
	~add_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class sub_instruct : public instruct {
public:
	~sub_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class mul_instruct : public instruct {
public:
	~mul_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class div_instruct : public instruct {
public:
	~div_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};
