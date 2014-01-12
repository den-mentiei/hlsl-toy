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
	parse_textures();

	return true;
}

void ToyFile::parse_textures() {
	_textures.clear();

	for (size_t i = 0; i < _code.length(); ++i) {
		// TODO: parse comments with texture filenames
	}
}

const char* ToyFile::code() const {
	return _code.c_str();
}

size_t ToyFile::code_length() const {
	return _code.length();
}

const wchar_t* ToyFile::texture_path(const unsigned id) const {
	return _textures[id].c_str();
}

unsigned ToyFile::n_textures() const {
	return _textures.size();
}

} // namespace toy