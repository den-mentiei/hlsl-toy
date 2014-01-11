#pragma once

#include <string>

namespace toy {

class ToyFile {
public:
	bool init(const wchar_t* path);

	const char* code() const;
	size_t code_length() const;
private:
	std::string _code;
};

} // namespace toy