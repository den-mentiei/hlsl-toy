#include "options.h"
#include "windows.h"

#include <cstdlib>

namespace toy {

Options::Options()
	: _valid(false)
	, _w(DEFAULT_RESOLUTION_W)
	, _h(DEFAULT_RESOLUTION_H)
{
	int argc;
	_argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

	if (argc < 2) {
		// There is no toy
		return;
	}

	_valid = true;

	// App name, toy file, width, height
	if (argc == 1 + 1 + 2) {
		// TODO: remove _wtoi crap
		const unsigned w = _wtoi(_argv[2]);
		const unsigned h = _wtoi(_argv[3]);

		if (w != 0 && h != 0) {
			_w = w;
			_h = h;
		}
	}
}

Options::~Options() {
	::LocalFree(_argv);
}

unsigned Options::w() const {
	return _w;
}

unsigned Options::h() const {
	return _h;
}

const wchar_t* Options::toy_file() const {
	return _argv[1];
}

bool Options::valid() const {
	return _valid;
}

} // namespace toy