#pragma once
#include <string>
#include "windows.h"

namespace toy {

class DumbFileMonitor {
public:
	DumbFileMonitor();
	~DumbFileMonitor();

	void start(LPCWSTR filename);
	void stop();

	bool changed() const;

private:
	std::wstring _filename;
	volatile LONG _version;
	volatile LONG _stop;
	mutable LONG _last_checked_version;
	HANDLE _thread;

	struct filetime_t {
		filetime_t(LPCWSTR filename);
		bool operator!=(const filetime_t &other);

		inline operator bool() const { return _valid; }
	private:
		FILETIME _time;
		bool _valid;
	};

	static DWORD WINAPI thread_main(LPVOID param);
	void thread_main();
};

} // namespace toy
