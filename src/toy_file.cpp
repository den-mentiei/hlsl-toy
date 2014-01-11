#include "toy_file.h"
#include <sstream>
#include <fstream>

namespace toy {

bool ToyFile::init(const wchar_t* path) {
	std::ifstream f(path, std::ifstream::in);

	if (!f.good()) {
		return false;
	}

	std::stringstream buffer;
	buffer << f.rdbuf();

	_code = buffer.str();

	return true;
}

const char* ToyFile::code() const {
	return _code.c_str();
}

size_t ToyFile::code_length() const {
	return _code.length();
}

} // namespace toy