#include "types.hpp"


object_type evaluate_type(object_type lhs_type, object_type rhs_type) {
	if (lhs_type == rhs_type) { return lhs_type; }
	if (lhs_type == object_type::none || rhs_type == object_type::none) {
		return object_type::none;
	}
	if ((lhs_type == object_type::integer && rhs_type == object_type::floating) ||
		(lhs_type == object_type::floating && rhs_type == object_type::integer)) {
		return object_type::floating;
	}
	return object_type::none;
}
