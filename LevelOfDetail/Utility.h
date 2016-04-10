#pragma once

#include <sstream>
#include <stdexcept>

// Converts a number to an ansi string
template<typename T> inline std::string ToAnsiString(const T& val)
{
	std::ostringstream stream;
	if (!(stream << val))
		throw std::runtime_error("Error converting value to string");
	return stream.str();
}