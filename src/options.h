#pragma once

namespace toy {

class Options {
public:
	enum {
		DEFAULT_RESOLUTION_W = 1280,
		DEFAULT_RESOLUTION_H = 720,
	};

	Options();
	~Options();

	const wchar_t* toy_file() const;
	unsigned w() const;
	unsigned h() const;

	bool valid() const;
private:
	wchar_t** _argv;
	unsigned _w, _h;
	bool _valid;
};

} // namespace toy