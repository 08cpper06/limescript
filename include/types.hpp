#pragma once
#include <string>

enum class object_type;

object_type evaluate_type(object_type lhs_type, object_type rhs_type);

std::string to_string(object_type type);
