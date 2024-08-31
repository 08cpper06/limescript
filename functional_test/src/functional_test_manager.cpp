#include "functional_test_manager.hpp"
#include <future>
#include <chrono>
#include <iostream>


functional_test_manager::functional_test_manager() {}
functional_test_manager::~functional_test_manager() {}

void functional_test_manager::internal_execute(
	unsigned int timeout,
	const std::unique_ptr<test_base_class>& ptr,
	const std::unique_ptr<void>& parameter
) {
	std::promise<unsigned int> p;
	std::future_status result;
	std::future<unsigned int> f = p.get_future();
	{
		std::thread th([&ptr, &parameter](std::promise<unsigned int> p) {
			std::chrono::steady_clock::time_point begin
				= std::chrono::steady_clock::now();
			
			bool is_success = ptr->run_test(parameter);

			std::chrono::steady_clock::time_point end
				= std::chrono::steady_clock::now();

			unsigned int time =
				std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
			p.set_value(time);

			if (!is_success) {
				std::cout << "Test is failed!!" << std::endl;
			} else {
				std::cout << "Test is success (" << time << " us)" << std::endl;
			}

		}, std::move(p));
		result = f.wait_for(std::chrono::milliseconds(timeout));
		if (result != std::future_status::timeout) {
			th.join();
		} else {
			th.detach(); /* FIXME: unmanaged thread!! */
		}
	}

	if (result == std::future_status::timeout) {
		std::cout << "Test is timeout" << std::endl;
	}

}

void functional_test_manager::execute(unsigned int timeout) {
	for (std::unique_ptr<test_base_class>& ptr : tests) {
		std::vector<test_parameter> params;
		ptr->get_tests(params);
		std::cout << "=== " << ptr->get_test_name()
					<< "(Count: " << params.size() << ") ===" << std::endl;
		for (const test_parameter& param : params) {
			std::cout << "--- Parameter: \"" << param.test_name
					<< "\" ---" << std::endl;
			internal_execute(timeout, ptr, param.object);
		}
	}
}

void functional_test_manager::add_test(std::unique_ptr<test_base_class> test) {
	tests.push_back(std::move(test));
}