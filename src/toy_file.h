#pragma once

namespace toy {

class ToyFile {
public:
	bool init(const wchar_t* path);

	const char* code() const;
	size_t code_length() const;
private:
	char* _code;
	size_t _code_length;
};

} // namespace toy