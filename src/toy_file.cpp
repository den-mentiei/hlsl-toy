#include "toy_file.h"
#include <locale>
#include <sstream>
#include <fstream>
#include <cassert>

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

	enum State {
		LINE_START, SLASH, COMMENT, TEXTURE_NAME, WAIT_FOR_LINE, END
	} state = LINE_START;

	size_t i = 0;
	const char* const s = _code.c_str();
	const char* texture_name_start = nullptr;
	std::locale locale;
	while (state != END) {
		switch (state) {
			case LINE_START:
				if (s[i] == '/') {
					state = SLASH;
				} else {
					state = WAIT_FOR_LINE;
				}
				i++;
				break;

			case SLASH:
				if (s[i] == '/') {
					state = COMMENT;
					i++;
				} else {
					state = WAIT_FOR_LINE;
				}
				break;

			case COMMENT:
				// skips whitespace
				if (s[i] == ' ' || s[i] == '\t') {
					i++;
				} else if (s[i] == '\n') {
					state = LINE_START;
					i++;
				} else if (s[i] == '\0') {
					state = END;
				} else {
					texture_name_start = s + i;
					state = TEXTURE_NAME;
					i++;
				}
				break;

			case TEXTURE_NAME:
				if (s[i] == '\n') {
					add_texture_path(texture_name_start, s + i);
					state = LINE_START;
					i++;
				} else if (std::isalnum(s[i], locale) || s[i] == '.' || s[i] == '_' || s[i] == '-') {
					i++;
				} else {
					state = WAIT_FOR_LINE;
				}
				break;

			case WAIT_FOR_LINE:
				if (s[i] == '\n') {
					state = LINE_START;
				} else if (s[i] == '\0') {
					state = END;
				}
				i++;
				break;
		}
	}
}

void ToyFile::add_texture_path(const char* const start, const char* const end) {
	assert(start != nullptr);
	assert(end != nullptr);
	assert(start < end);

	_textures.push_back(std::wstring(start, end));
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