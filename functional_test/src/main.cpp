#include "functional_test_manager.hpp"


int main() {
	functional_test_manager::get_instance()->execute(10'000);
	return 0;
}
