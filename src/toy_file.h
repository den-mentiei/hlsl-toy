#pragma once

#include <string>
#include <vector>

namespace toy {

class ToyFile {
public:
	bool init(const wchar_t* path);

	const char* code() const;
	size_t code_length() const;

	const wchar_t* texture_path(const unsigned id) const;
	unsigned n_textures() const;
private:
	void parse_textures();
	void add_texture_path(const char* const start, const char* const end);

	std::vector<std::wstring> _textures;
	std::string _code;
};

} // namespace toy