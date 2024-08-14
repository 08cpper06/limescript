#pragma once


enum class object_type {
	none,
	integer,
	floating,
};


object_type evaluate_type(object_type lhs_type, object_type rhs_type);
