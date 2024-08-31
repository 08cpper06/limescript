#include "functional_test_manager.hpp"
#include "tokenize.hpp"
#include "parser.hpp"
#include "asm.hpp"
#include <filesystem>
#include <fstream>


IMPLEMENT_FUNCTIONAL_TEST(build_ast)
void build_ast_test::get_tests(std::vector<test_parameter>& parameters) const {
	namespace fs = std::filesystem;
	std::string path = "../../functional_test/test";
	for (const fs::directory_entry& entry : fs::directory_iterator(path + "/source")) {
		std::string param;
		param = entry.path().string() + ";";
		param += path + "/parse/" + entry.path().filename().replace_extension("par").string();
		parameters.push_back(test_parameter {
			.test_name = entry.path().filename().string(),
			.object = std::make_unique<std::string>(param.c_str())
		});
	}
}
bool build_ast_test::run_test(const std::unique_ptr<void>& parameter) const {
	std::string* param = static_cast<std::string*>(parameter.get());
	size_t pos = param->find(";");
	std::string src_file = param->substr(0, pos);
	std::string res_file = param->substr(pos + 1);

	std::ifstream in(src_file);
	std::string text((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
	text.push_back('\0');

	std::vector<token> tokens = lexer::tokenize(std::move(text));
	std::unique_ptr<ast_base_node> root = parser::parse(std::move(tokens));
	if (!root) {
		return false;
	}
	in = std::ifstream(res_file);
	if (in.fail()) {
		return false;
	}
	text = std::string((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));
	std::string test_str = root->log("");
	if (text != test_str) {
		return false;
	}

	return true;
}

struct return_test_parameter {
	return_test_parameter() = default;
	return_test_parameter(OBJECT ret, const std::string& source) :
		return_value(ret),
		source(source)
	{}
	OBJECT return_value;
	std::string source;
};

struct cmp_not_equal {
	bool operator()(const int& x, const int& y) { return x != y; }
	bool operator()(const double& x, const double& y) { return x != y; }
	bool operator()(const std::string& x, const std::string& y) { return x != y; }
	template <class Type, class UType>
	bool operator()(Type, UType) { return true; }
};

IMPLEMENT_FUNCTIONAL_TEST(runtime_execute)
void runtime_execute_test::get_tests(std::vector<test_parameter>& parameters) const {
	parameters.push_back(
		test_parameter {
			.test_name = "return integer value",
			.object = std::make_unique<return_test_parameter>(OBJECT(3), "return 1 + 2;")
		}
	);
	parameters.push_back(
		test_parameter {
			.test_name = "return integer value with variable",
			.object = std::make_unique<return_test_parameter>(OBJECT(3), "const v: int = 1; return v + 2;")
		}
	);
	parameters.push_back(
		test_parameter {
			.test_name = "return floating value",
			.object = std::make_unique<return_test_parameter>(OBJECT(3.14), "return 1 + 2.14;")
		}
	);
	parameters.push_back(
		test_parameter {
			.test_name = "return floating value with variable",
			.object = std::make_unique<return_test_parameter>(OBJECT(3.14), "const v: float = 2.14; return 1 + v;")
		}
	);
}
bool runtime_execute_test::run_test(const std::unique_ptr<void>& parameter) const {
	return_test_parameter* param = static_cast<return_test_parameter*>(parameter.get());
	std::vector<token> tokens = lexer::tokenize(param->source);
	std::unique_ptr<ast_base_node> root = parser::parse(std::move(tokens));
	if (!root) {
		return false;
	}
	asm_context con;
	root->encode(con);
	for (const std::unique_ptr<instruct>& inst : con.codes) {
		inst->execute(con);
		if (con.is_abort) {
			break;
		}
	}

	if (con.stack.size() != 1) {
		return false;
	}
	if (con.stack.back().value.index() != param->return_value.index()) {
		return false;
	}

	if (std::visit(cmp_not_equal{}, con.stack.back().value, param->return_value)) {
		return false;
	}
	return true;
}
