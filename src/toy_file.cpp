#include "toy_file.h"

namespace toy {

bool ToyFile::init(const wchar_t* path) {
	return false;
}

const char* ToyFile::code() const {
	return _code;
}

size_t ToyFile::code_length() const {
	return _code_length;
}

} // namespace toy