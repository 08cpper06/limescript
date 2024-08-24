#pragma once
#include <list>
#include <memory>
#include <variant>
#include <string>
#include <map>
#include "types.hpp"


struct invalid_type {};
#define OBJECT std::variant<invalid_type, int, double, std::string>

static inline constexpr int INVALID_TYPE_INDEX = OBJECT(invalid_type()).index();
static inline constexpr int INT_TYPE_INDEX = OBJECT(0).index();
static inline constexpr int DOUBLE_TYPE_INDEX = OBJECT(0.).index();
static inline constexpr int STRING_TYPE_INDEX = OBJECT("").index();

enum class object_type {
	none = INVALID_TYPE_INDEX,
	integer = INT_TYPE_INDEX,
	floating = DOUBLE_TYPE_INDEX,
	string = STRING_TYPE_INDEX,
};

class instruct;
class operand;

struct variable {
	std::string name;
	bool is_mutable;
	bool is_init { false };
	OBJECT value;
};

struct asm_context {
	std::list<operand> stack;
	bool is_abort { false };

	std::map<std::string, variable> variables;

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
	variable,
};

struct operand {
	operand_type type;
	OBJECT value;
};

class push_instruct : public instruct {
public:
	~push_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	operand value;
};

class pop_instruct : public instruct {
public:
	~pop_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class alloc_instruct : public instruct {
public:
	~alloc_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	bool is_mutable { false };
	std::string name;
	object_type type;
};

class init_instruct : public instruct {
public:
	~init_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	std::string lhs;
};

class return_instruct : public instruct {
public:
	~return_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class abort_instruct : public instruct {
public:
	~abort_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class mov_instruct : public instruct {
public:
	~mov_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	std::string lhs;
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

class movf_instruct : public instruct {
public:
	~movf_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	std::string lhs;
};

class addf_instruct : public instruct {
public:
	~addf_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class subf_instruct : public instruct {
public:
	~subf_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class mulf_instruct : public instruct {
public:
	~mulf_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class divf_instruct : public instruct {
public:
	~divf_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;
};

class cast_instruct : public instruct {
public:
	cast_instruct(object_type type);
	~cast_instruct() = default;
	void execute(asm_context& con) const override;
	std::string log(const std::string& prefix) const override;

public:
	object_type to;
};
