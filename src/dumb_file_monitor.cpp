#include "dumb_file_monitor.h"

namespace toy {

DumbFileMonitor::DumbFileMonitor() : _thread(INVALID_HANDLE_VALUE) {}

DumbFileMonitor::~DumbFileMonitor() { stop(); }

void DumbFileMonitor::start(LPCWSTR filename) {
	if (_thread != INVALID_HANDLE_VALUE) return;

	_filename = filename;
	_stop = _last_checked_version = _version = 0;
	_thread = CreateThread(NULL, 0, thread_main, this, 0, NULL);
}

void DumbFileMonitor::stop() {
	if (_thread == INVALID_HANDLE_VALUE) return;

	InterlockedIncrement(&_stop);
	WaitForSingleObject(_thread, INFINITE);
	CloseHandle(_thread);
	_thread = INVALID_HANDLE_VALUE;
}

bool DumbFileMonitor::changed() const {
	const LONG version = _version;
	bool changed = (_last_checked_version != version);
	_last_checked_version = version;
	return changed;
}

DWORD DumbFileMonitor::thread_main(LPVOID param) {
	DumbFileMonitor *self = reinterpret_cast<DumbFileMonitor*>(param);
	self->thread_main();
	return 0;
}

void DumbFileMonitor::thread_main() {
	filetime_t prev(_filename.c_str());
	if (!prev) return;

	while(_stop == 0) {
		Sleep(1000);
		filetime_t now(_filename.c_str());
		if (now && now != prev) {
			InterlockedIncrement(&_version);
			prev = now;
		}
	}
}

DumbFileMonitor::filetime_t::filetime_t(LPCWSTR filename) : _valid(false) {
	HANDLE file = CreateFileW(filename, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return;

	_valid = TRUE == GetFileTime(file, NULL, NULL, &_time);
	CloseHandle(file);
}

bool DumbFileMonitor::filetime_t::operator!=(const filetime_t &other) {
	return _valid && (
		other._time.dwHighDateTime != _time.dwHighDateTime ||
		other._time.dwLowDateTime != _time.dwLowDateTime);
}

} // namespace toy
