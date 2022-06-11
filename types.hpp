#ifndef VALUE_AND_TYPE_HPP
#define VALUE_AND_TYPE_HPP

#include <cstdint>

using IntType = int64_t;

enum class DataType: uint8_t{
	NONE,

	LIST,
	INTEGER,
	VARIABLE
};

#endif	// VALUE_AND_TYPE_HPP