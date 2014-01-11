#pragma once

namespace toy {

class Timer {
public:
	Timer();

	void tick();

	double delta() const;
	double elapsed() const;
private:
	double _frequency;
	double _elapsed;
	double _delta;
	unsigned long long _time;
	unsigned long long _prev_time;
};

} // namespace toy