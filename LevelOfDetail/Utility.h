#pragma once

#include <sstream>
#include <stdexcept>
#include <comdef.h> //_com_error type

// Converts a number to an ansi string
template<typename T> inline std::string ToAnsiString(const T& val)
{
	std::ostringstream stream;
	if (!(stream << val))
		throw std::runtime_error("Error converting value to string");
	return stream.str();
}

static std::string GetErrorMessageFromHRESULT(HRESULT hr)
{
	_com_error err(hr);
	std::wstring ws = err.ErrorMessage();
	return std::string(ws.begin(), ws.end());
}

static std::wstring StrToWStr(std::string s)
{
	return std::wstring(s.begin(), s.end());
}
