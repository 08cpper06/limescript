#pragma once

#include <vector>
#include <memory>
#include <string>


class test_base_class;

template <>
struct std::default_delete<void> {
	constexpr default_delete() noexcept = default;

	template <class _Ty2, enable_if_t<is_convertible_v<_Ty2*, void*>, int> = 0>
	constexpr default_delete(const default_delete<_Ty2>&) noexcept {}

	constexpr void operator()(void* _Ptr) const noexcept {
		delete _Ptr;
	}
};

struct test_parameter {
	std::string test_name;
	std::unique_ptr<void> object;
};

class functional_test_manager {
public:
	static functional_test_manager* get_instance() {
		static functional_test_manager instance;
		return &instance;
	}

	void execute(unsigned int timeout);

	void add_test(std::unique_ptr<test_base_class> test);

private:
	void internal_execute(
		unsigned int timeout,
		const std::unique_ptr<test_base_class>& ptr,
		const std::unique_ptr<void>& parameter
	);
	functional_test_manager();
	~functional_test_manager();

	std::vector<std::unique_ptr<test_base_class>> tests;
};

class test_base_class {
public:
	test_base_class() = default;
	virtual ~test_base_class() = default;

	virtual std::string get_test_name() const noexcept = 0;
	virtual void get_tests(std::vector<test_parameter>& parameters) const = 0;
	virtual bool run_test(const std::unique_ptr<void>& parameter) const = 0;
};

#define IMPLEMENT_FUNCTIONAL_TEST(TestName) class TestName##_test : \
	public test_base_class { \
public: \
	TestName##_test() noexcept {} \
	~TestName##_test() noexcept = default; \
	TestName##_test(decltype(nullptr)) noexcept { \
		std::unique_ptr<test_base_class> ptr = std::make_unique<TestName##_test>();\
		functional_test_manager::get_instance()->add_test(std::move(ptr)); \
	} \
	std::string get_test_name() const noexcept { return #TestName; } \
	void get_tests(std::vector<test_parameter>& parameters) const; \
	bool run_test(const std::unique_ptr<void>& parameter) const; \
}; static TestName##_test TestName##test_instance(nullptr);
