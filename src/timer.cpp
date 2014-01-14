#include "timer.h"

#include "window.h"

namespace toy {

Timer::Timer()
	: _elapsed(0.0)
	, _delta(0.0)
{
	LARGE_INTEGER freq;
	::QueryPerformanceFrequency(&freq);
	_frequency = double(freq.QuadPart);
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	_prev_time = counter.QuadPart;
}

void Timer::tick() {
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);

	_time = counter.QuadPart;
	_delta = (_time - _prev_time) / _frequency;
	_elapsed += _delta;
	_prev_time = _time;
}

double Timer::delta() const {
	return _delta;
}

double Timer::elapsed() const {
	return _elapsed;
}

} // namespace toy